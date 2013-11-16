import java.io.IOException;
import java.util.*;

import org.oescript.api.*;
import org.oescript.msg.Message;
import org.oescript.java.swig.*;
import org.oescript.javaclient.ExtentClient;
import org.oescript.javaclient.SocketClient;
import org.oescript.util.StringParse;

public class test_json_server {

    static String EXTNAME  = "myspace";
    static String SERVER_STR = "oejson://0.0.0.0:6789/" + EXTNAME;
    static String SADDR_STR  = "127.0.0.1";
    static int    SPORT      = 6789;

    static {
        System.loadLibrary("oescriptimpl");
    }

    public static void main(String[] args) throws Exception {

        test_string_parse();

        System.out.println("json server test starting...");
        startServer();
        //Thread.sleep(30000);
        test_socket_client();
        test_extent_client();
        connect_extent_client();

        test_put_list();
        test_bad_get_list();
        test_read_list();
        test_take_list();
        test_take_null_list();

        /* test_put_map();
        test_bad_get_map();
        test_read_map();
        test_take_map(); */

        //test_take_null_map();

        test_begin_txn();
        test_put_list();
        test_take_null_list();
        test_commit_txn();
        test_take_list();

        test_begin_txn();
        test_put_list();
        test_take_null_list();
        test_abort_txn();
        test_take_null_list();

        test_put_list_with_bytes();
        test_read_list_with_bytes();

        disconnect_extent_client();
        stopServer();
        System.out.println("...json server test done.");
    }

    static OesKernel _kernel;
    static OesStore _store;
    static OesDispatcher _dispatcher;
    static OesThreadDispatcher _tdispatcher;

    static void startServer() {

        OesSigHandler signals = null;
        assert(signals == null);
        signals = new OesSigHandler("mysigfile.txt", "oesjava", "info");
        assert(signals != null);

        _dispatcher = new OesDispatcher(1, "mydispatcher");
        assert(_dispatcher != null);
        _dispatcher.start();

        _tdispatcher = new OesThreadDispatcher(4);
        assert(_tdispatcher != null);
        _tdispatcher.start();

        _store = new OesStore(3, "data", true);
        assert(_store != null);

        _kernel = new OesKernel(_dispatcher, _tdispatcher, _store,
                                null, null, null, null);
        assert(_kernel != null);

        //start socket server
        OesNet net = new OesNet(_dispatcher, false, null, null);
        OesServer s = new OesServer(_dispatcher, SERVER_STR, net, _store);
        s.addKernel(EXTNAME, _kernel);
        
    }

    static void stopServer() {
        _dispatcher.delete();
        _tdispatcher.delete();
        _kernel.delete();
        _store.delete();
    }
    static void test_socket_client() throws IOException {
        SocketClient sc = new SocketClient();
        sc.connect(SADDR_STR, SPORT);
        sc.disconnect();
    }
    static void test_string_parse() throws IOException {
        String uri = "jcl://127.0.0.1:5555/yourjavaspace";
        int p = StringParse.parsePort(uri, -1);
        System.out.println("test port parse: " + p);
        assert (p == 5555);
        String a = null;
        a = StringParse.parseAddr(uri);
        System.out.println("test addr parse: " + a);
        assert ("127.0.0.1".equals(a));
        String t = null;
        t = StringParse.parseProto(uri);
        System.out.println("test proto parse: " + t);
        assert ("jcl".equals(t));
        String e = null;
        e = StringParse.parseExtentname(uri);
        System.out.println("test extentname parse: " + e);
        assert ("yourjavaspace".equals(e));
    }
    static void test_extent_client() throws IOException {
        int p = StringParse.parsePort(SERVER_STR, -1);
        assert (p != -1);
        String a = StringParse.parseAddr(SERVER_STR);
        assert (a != null);
        String t = StringParse.parseProto(SERVER_STR);
        assert (t != null);
        ExtentClient ec = ExtentClient.getClient(SERVER_STR);
        ec.connect();
        ec.disconnect();
    }
    static ExtentClient _ec;
    static void connect_extent_client() throws IOException {
        _ec = ExtentClient.getClient(SERVER_STR);
        _ec.setMsgHook(new MyMsgHook());
        _ec.connect();
        System.out.println("client is connected: " + _ec.isConnected());
    }
    static void disconnect_extent_client() throws IOException {
        _ec.disconnect();
        _ec = null;
    }
    static void test_put_list() throws IOException {
        List l = new ArrayList();
        l.add("one");
        l.add("two");
        l.add("three");
        Lease lease = _ec.writeList(l);
        assert(lease != null);
        assert(lease.getId() != null);
    }
    static void test_put_map() throws IOException {
        Map m = new HashMap();
        m.put("k1", "one");
        m.put("k2", "two");
        m.put("k3", "three");
        Lease lease = _ec.writeMap(m);
        assert(lease != null);
        assert(lease.getId() != null);
    }
    static void test_bad_get_list() throws IOException {
        List t = new ArrayList();
        t.add("four");
        t.add(null);
        t.add(null);
        long to = _ec.getTimeout();
        _ec.setTimeout(0);
        List rl = _ec.readList(t);
        _ec.setTimeout(to);
        assert(rl == null);
    }
    static void test_bad_get_map() throws IOException {
        Map t = new HashMap();
        t.put("k1", "two");
        long to = _ec.getTimeout();
        _ec.setTimeout(0);
        Map rm = _ec.readMap(t);
        _ec.setTimeout(to);
        assert(rm == null);
    }
    /*
    static void test_bad_get_list() throws IOException {
        List t = new ArrayList();
        t.add("one");
        t.add(null);
        t.add(null);
        boolean got_exception = false;
        try {
            _ec.readList(t);
        } catch (IOException e) {
            System.out.println("yay: " + e.getMessage());
            got_exception = true;
            disconnect_extent_client();
            connect_extent_client();
        }
        assert(got_exception);
    }
    */
    static void test_take_list() throws IOException {
        List t = new ArrayList();
        t.add("_");
        t.add("two");
        t.add("_");
        List<String> l = _ec.takeList(t);
        assert(l != null);
        assert(l.size() == 3);
        assert(l.get(0).equals("one"));
        assert(l.get(1).equals("two"));
        assert(l.get(2).equals("three"));
    }
    static void test_take_null_list() throws IOException {
        List t = new ArrayList();
        t.add("_");
        t.add("two");
        t.add("_");
        long to = _ec.getTimeout();
        _ec.setTimeout(0);
        List<String> l = _ec.takeList(t);
        _ec.setTimeout(to);
        assert(l == null);
    }
    static void test_read_list() throws IOException {
        List t = new ArrayList();
        t.add("_");
        t.add("two");
        t.add("_");
        List<String> l = _ec.readList(t);
        assert(l != null);
        assert(l.size() == 3);
        assert(l.get(0).equals("one"));
        assert(l.get(1).equals("two"));
        assert(l.get(2).equals("three"));
    }
    static String TEST_BYTES = "i am two bytes!";
    static void test_put_list_with_bytes() throws IOException {
        List l = new ArrayList();
        l.add("won");
        l.add("too");
        l.add("thrie");
        _ec.setAttachment(TEST_BYTES.getBytes("UTF8"));
        Lease lease = _ec.writeList(l);
        assert(lease != null);
        assert(lease.getId() != null);
    }
    static void test_read_list_with_bytes() throws IOException {
        List t = new ArrayList();
        t.add("won");
        t.add("too");
        t.add("_");
        List<String> l = _ec.readList(t);
        String byteString = new String(_ec.getAttachment(), "UTF8");
        assert(byteString != null);
        assert(byteString.equals(TEST_BYTES));
        assert(l != null);
        assert(l.size() == 3);
        System.out.println("1: " + l.get(0));
        System.out.println("2: " + l.get(1));
        System.out.println("3: " + l.get(2));
        assert(l.get(0).equals("won"));
        assert(l.get(1).equals("too"));
        assert(l.get(2).equals("thrie"));
    }
    static void test_read_map() throws IOException {
        Map t = new HashMap();
        t.put("k1", "one");
        Map<String, String> rm = _ec.readMap(t);
        assert(rm != null);
        assert(rm.size() == 3);
        assert(rm.get("k1").equals("one"));
        assert(rm.get("k2").equals("two"));
        assert(rm.get("k3").equals("three"));
        assert(rm.get("k4") == null);
    }
    static void test_take_map() throws IOException {
        Map t = new HashMap();
        t.put("k1", "one");
        Map<String, String> rm = _ec.takeMap(t);
        assert(rm != null);
        assert(rm.size() == 3);
        assert(rm.get("k1").equals("one"));
        assert(rm.get("k2").equals("two"));
        assert(rm.get("k3").equals("three"));
        assert(rm.get("k4") == null);
    }
    static void test_begin_txn() throws IOException {
        long now = System.currentTimeMillis();
        Txn txn = _ec.begin(60 * 1000 * 10);
        assert(txn != null);
        //assert(txn.getExptime() > now);
    }
    static void test_commit_txn() throws IOException {
        long now = System.currentTimeMillis();
        Txn txn = _ec.getCurrentTxn();
        assert(txn != null);
        assert(txn.getStatus() == TxnStatus.ACTIVE);
        txn.commit();
        txn = _ec.getCurrentTxn();
        assert(txn == null);
    }
    static void test_abort_txn() throws IOException {
        long now = System.currentTimeMillis();
        Txn txn = _ec.getCurrentTxn();
        assert(txn != null);
        assert(txn.getStatus() == TxnStatus.ACTIVE);
        txn.rollback();
        txn = _ec.getCurrentTxn();
        assert(txn == null);
    }

    static private class MyMsgHook implements ExtentMsgHook {

        public void logMsg(Message m, String s) {
            //System.out.println("got:\n" + s);
        }
    }
}

