package org.oescript.javaclient;
import java.io.IOException;
import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.oescript.api.*;
import org.oescript.msg.*;
import org.oescript.util.StringParse;

//////////////////////////////////
//////////////////////////////////
// class is a *mess* ejs todo:  //
// consolodate the lazy connect //
// logic                        //
//////////////////////////////////
//////////////////////////////////


public class ExtentClient implements Extent {

    private ExtentMsgHook _hook;
    private EcTxn _curr_txn;
    private byte[] _bytes;
    private final boolean _lazy_connect;
    private long _timeout;
    private long _lease_dur;
    private String _connect_str;
    private SocketClient _net_client;
    private MessageCoder _coder;
    private boolean _is_connected = false;
    private ConResponse _con_res;
    private static IdFactory _id_factory = new IdFactory();
    private final String _server_spec, _username, _pwd;
    private final String _saddr, _sproto, _extname;
    private final int _sport;
    private static final Map<String, ExtentClient> _registry;
    static {
        Map<String, ExtentClient> r = new HashMap<String, ExtentClient>();
        _registry = Collections.synchronizedMap(r);
    }

    public static ExtentClient getClient(String server_spec) 
    throws java.net.MalformedURLException {

        synchronized (_registry) { 
            ExtentClient c = _registry.get(server_spec);
            if (c != null)  return c;
            c = new ExtentClient(server_spec);
            _registry.put(server_spec,c);
            return c;
        }
    }

    public ExtentClient(String server_spec) throws java.net.MalformedURLException {
        this(server_spec, "default", "secret", false);
    }

    public ExtentClient(String server_spec, 
                        String username, 
                        String pwd, 
                        boolean lazyConnect) throws java.net.MalformedURLException {

        _lazy_connect = lazyConnect;
        _username = username;
        _pwd = pwd;

        _server_spec = server_spec;

        _saddr = StringParse.parseAddr(server_spec);
        if (_saddr == null)
            throw new java.net.MalformedURLException("can not parse address");

        _sport = StringParse.parsePort(server_spec, -1);
        if (_sport == -1)
            throw new java.net.MalformedURLException("can not parse port");

        _sproto = StringParse.parseProto(server_spec);
        if (_sproto == null || !_sproto.equals("oejson"))
            throw new java.net.MalformedURLException("unsupported or invalid protocol");

        _extname = StringParse.parseExtentname(server_spec);
        if (_extname == null)
            throw new java.net.MalformedURLException("can not parse extentname");

        _coder = new JsonMessageCoder();
        _timeout = 30 * 1000;
        _lease_dur = 60 * 1000 * 10;
    }

    private synchronized boolean readConRes() throws IOException {
        ConResponse res = (ConResponse) read();
        if (res == null) {
            throw new IOException("no con response");
        }
        if (res.getSid() != null && res.getSid() != Message.NILID) {
            _is_connected = true;
        }
        return _is_connected;
    }
    private synchronized Response readRes() throws IOException {
        if (!_is_connected && _lazy_connect) {
            readConRes();
        }
        return(Response) read();
    }
    private synchronized Message read() throws IOException {
        Message resmsg = null;
        String rstr = _net_client.readString(); 
        if (rstr == null) return null;
        try {
            resmsg = _coder.fromString(rstr);
            if (_hook != null) {
                _hook.logMsg(resmsg,rstr);
            }
        } catch (MessageCoderException mce) {
            throw new IOException(mce);
        }
        if (resmsg instanceof org.oescript.msg.Error) {
            throw new IOException(((org.oescript.msg.Error) resmsg).getMsg());
        }
        return resmsg;
    }
    private String getOutputStr(Message msg) throws MessageCoderException {
        StringBuffer b = new StringBuffer(100);
        if (_lazy_connect && !_is_connected) {
            ConCommand c = createConCmd();
            String constr = _coder.toString(c);
            if (_hook != null) {
                _hook.logMsg(c,constr);
            }
            b.append(constr);
            b.append("\n\n");
        }
        String mstr = _coder.toString(msg);
        if (_hook != null) {
            _hook.logMsg(msg,mstr);
        }
        b.append(mstr);
        b.append("\n\n");
        return b.toString();
    }
    private void incrementEnlisted(int i) {
        if (_curr_txn != null) {
            _curr_txn.increment(i);
        }
    }
    private synchronized void writeMsg(Message msg) throws IOException {
        if (_lazy_connect && !_is_connected) {
            connectSocket();
        }
        String s = getOutputStr(msg);
        _net_client.writeString(s);
    }

    private ConCommand createConCmd() {
        ConCommand con = new ConCommand();
        con.setCid(_id_factory.nextId());
        con.setExtentName(_extname);
        con.setMaj(0);
        con.setMaj(4);
        con.setMaj(0);
        con.setUsername(_username);
        con.setPwd(_pwd);
        return con;
    }

    private synchronized void connectSocket() throws IOException {
        if (_net_client != null)
            throw new IOException("already connected");
        _net_client = new SocketClient();
        _net_client.connect(_saddr, _sport);
    }

    public synchronized Extent connect() throws IOException {
        if (_net_client != null)
            throw new IOException("already connected");
        if (_lazy_connect)  return this;
        connectSocket();
        String mstr;
        ConCommand con = createConCmd();
        writeMsg(con);
        ConResponse res = (ConResponse) readRes();
        if (res.getSid() != null && res.getSid() != Message.NILID) {
            _is_connected = true;
        }
        _con_res = res;
        return this;
    }

    public synchronized Extent disconnect() throws IOException {
        if (_net_client == null) return this;
        _net_client.disconnect();
        _is_connected = false;
        _net_client = null;
        synchronized (_registry) { 
            _registry.remove(_server_spec);
        }
        return this;
    }

    public synchronized Txn begin(long dur) throws IOException {
        StaCommand cmd = new StaCommand();
        cmd.setCid(_id_factory.nextId());
        cmd.setLeaseDur(dur);
        writeMsg(cmd);
        StaResponse res = (StaResponse) readRes();
        if (res == null) {
            throw new IOException("could not begin txn");
        }
        Txn txn = new EcTxn(res.getTid(), res.getExptime());
        setCurrentTxn(txn);
        return txn;
    }
    private synchronized Extent resetCurrentTxn(EcTxn txn) {
        if (_curr_txn == txn) {
            _curr_txn = null;
        }
        return this;
    }
    public synchronized Extent setCurrentTxn(Txn txn) {
        _curr_txn = (EcTxn) txn;
        return this;
    }
    public synchronized Txn getCurrentTxn() {
        return _curr_txn;
    }

    public synchronized Extent setLeaseDur(long dur) {
        _lease_dur = dur;
        return this;
    }
    public synchronized long getLeaseDur() {
        return _lease_dur;
    }
    public synchronized Extent setTimeout(long timeout) {
        _timeout = timeout;
        return this;
    }
    public synchronized long getTimeout() {
        return _timeout;
    }

    public synchronized Extent setAttachment(byte[] bytes) {
        _bytes = bytes;
        return this;
    }
    public synchronized byte[] getAttachment() {
        return _bytes;
    }

    public synchronized Lease writeList(List list) throws IOException {
        PutCommand cmd = new PutCommand();
        cmd.setCid(_id_factory.nextId());
        if (_curr_txn != null) {
            cmd.setTid(_curr_txn.getId());
        }
        cmd.setIsTuple(true);
        cmd.setLeaseDur(_lease_dur);
        cmd.getAttrs().addAll(list);
        if (_bytes != null) {
            cmd.setBytes(_bytes);
        }
        writeMsg(cmd);
        PutResponse res = (PutResponse) readRes();
        incrementEnlisted(1);
        return new EcLease(res.getLeaseId(), res.getExptime());
    }
    public List<String> readList(List<String> templ) throws IOException {
        return getList(templ, false);
    }
    public List<String> takeList(List<String> templ) throws IOException {
        return getList(templ, true);
    }
    private synchronized List<String> getList(List<String> templ, 
                                              boolean remove) throws IOException {
        GetCommand cmd = new GetCommand();
        cmd.setCid(_id_factory.nextId());
        if (_curr_txn != null) {
            cmd.setTid(_curr_txn.getId());
        }
        cmd.setTimeoutDur(_timeout);
        cmd.setIsTuple(true);
        cmd.setIsRemove(remove);
        cmd.setReturnAttrs(true);
        cmd.getAttrs().add(templ);
        writeMsg(cmd);
        GetResponse res = (GetResponse) readRes();
        if (remove) incrementEnlisted(res.getNResults());
        List attrs = res.getAttrs();
        if (attrs.size() == 0) {
            return null;
        }
        return attrs;
    }

    public synchronized Lease writeMap(Map<String, String> map) throws IOException {
        PutCommand cmd = new PutCommand();
        cmd.setCid(_id_factory.nextId());
        if (_curr_txn != null) {
            cmd.setTid(_curr_txn.getId());
        }
        cmd.setIsTuple(false);
        cmd.setLeaseDur(_lease_dur);
        List attrs = cmd.getAttrs();
        for (Map.Entry<String, String> e : map.entrySet()) {
            attrs.add(e.getKey());
            attrs.add(e.getValue());
        }
        if (_bytes != null) {
            cmd.setBytes(_bytes);
        }
        writeMsg(cmd);
        PutResponse res = (PutResponse) readRes();
        incrementEnlisted(1);
        return new EcLease(res.getLeaseId(), res.getExptime());
    }
    public Map<String,String> readMap(Map<String, String> templ) throws IOException {
        return getMap(templ, false);
    }
    public Map<String,String> takeMap(Map<String, String> templ) throws IOException {
        return getMap(templ, true);
    }
    private synchronized Map<String,String> getMap(Map<String, String> templ, 
                                                   boolean remove) throws IOException {
        GetCommand cmd = new GetCommand();
        cmd.setCid(_id_factory.nextId());
        if (_curr_txn != null) {
            cmd.setTid(_curr_txn.getId());
        }
        cmd.setTimeoutDur(_timeout);
        cmd.setIsTuple(false);
        cmd.setIsRemove(remove);
        cmd.setReturnAttrs(true);
        List<String> a = new ArrayList<String>();
        for (Map.Entry<String, String> e : templ.entrySet()) {
            a.add(e.getKey());
            a.add(e.getValue());
        }
        cmd.getAttrs().add(a);
        writeMsg(cmd);
        GetResponse res = (GetResponse) readRes();
        if (remove) incrementEnlisted(res.getNResults());
        _bytes = res.getBytes();
        List attrs = res.getAttrs();
        if (attrs.size() == 0) {
            return null;
        }
        Map results = new HashMap();
        for (Iterator iter = attrs.iterator(); iter.hasNext();) {
            results.put(iter.next(), iter.next());
        }
        return results;
    }

    public synchronized boolean isConnected() {
        return _is_connected;
    }

    private class EcLease implements org.oescript.api.Lease {
        private final Id _lid;
        private long _exptime;

        EcLease(Id lid, long exptime) {
            _lid = lid;
            _exptime = exptime;
        }
        public Extent getExtent() {
            return ExtentClient.this;
        }
        public Lease cancel() throws IOException {
            setDur(0);
            return this;
        }
        public synchronized long getExptime() {
            return _exptime;
        }
        public Id getId() {
            return _lid;
        }
        public synchronized Lease setDur(long newdur) throws IOException {
            UplCommand cmd = new UplCommand();
            cmd.setCid(_id_factory.nextId());
            cmd.setLeaseDur(newdur);
            cmd.setLeaseId(_lid);
            writeMsg(cmd);
            UplResponse res = (UplResponse) readRes();
            _exptime = res.getExptime();
            return this;
        }
    }
    //helper for clients that may persist txn info but need reboot new instance
    public Txn createTxn(Id tid, long exptime, TxnStatus status, Extent extent) {
        return new EcTxn(tid, exptime, status, extent);
    }
    private class EcTxn implements org.oescript.api.Txn {

        private Id _tid;
        //private long _exptime;
        private TxnStatus _status;
        private final Extent _extent;
        private final int _hc;
        private int _enlisted;

        EcTxn(Id tid, long exptime) {
            this(tid, exptime, TxnStatus.ACTIVE, ExtentClient.this);
        }
        EcTxn(Id tid, long exptime, TxnStatus status, Extent extent) {
            _tid = tid;
            _hc = ((new Long(tid.getLong())).hashCode()) + EcTxn.class.hashCode();
            //_exptime = exptime;
            _status = status;
            _extent = extent;
            _enlisted = 0;
        }
        synchronized void increment(int i) {
            _enlisted += i;
        }
        public Extent getExtent() {
            return _extent;
        }
        /*
        public synchronized long getExptime() {
            return _exptime;
        }
        */
        public synchronized Id getId() {
            return _tid;
        }
        public synchronized TxnStatus getStatus() {
            return _status;
        }
        private synchronized Txn update(TxnStatus newstatus) throws IOException {
            UptCommand cmd = new UptCommand();
            cmd.setCid(_id_factory.nextId());
            cmd.setTid(_tid);
            cmd.setStatus(newstatus);
            cmd.setEnlisted(_enlisted);
            writeMsg(cmd);
            UptResponse res = (UptResponse) readRes();
            if (res != null) {
                //_exptime = res.getExptime();
                _status = res.getStatus();
            }
            if (_status != newstatus) {
                throw new IOException("txn status not updated");
            }
            if (_status != TxnStatus.ACTIVE) { //if done, you are not curr anymore
                resetCurrentTxn(this);
            }
            return this;
        }
        public Txn commit() throws IOException {
            return update(TxnStatus.COMMITTED);
        }
        public Txn rollback() throws IOException {
            return update(TxnStatus.ROLLEDBACK);
        }
        public boolean equals(Object other) {
            if (other != null && 
                other instanceof EcTxn &&
                ((EcTxn) other)._tid.equals(_tid)) {
                return true;
            }
            return false;
        }
        public int hashCode() {
            return _hc;
        }
    }
    //for tools and debugging
    public void setMsgHook(ExtentMsgHook hook) {
        _hook = hook;
    }
}

