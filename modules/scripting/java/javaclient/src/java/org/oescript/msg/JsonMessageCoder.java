package org.oescript.msg;

import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.util.*;

import javax.xml.transform.Templates;

import org.oescript.api.Id;
import org.oescript.api.TxnStatus;
import org.oescript.jsonsimple.JSONArray;
import org.oescript.jsonsimple.JSONObject;
import org.oescript.jsonsimple.parser.JSONParser;
import org.oescript.util.Base64;

public class JsonMessageCoder implements MessageCoder {

    static final String OEJSON_CID                  = "cid";
    static final String OEJSON_TID                  = "tid";
    static final String OEJSON_TYPE                 = "t";
    static final String OEJSON_NAME                 = "n";
    static final String OEJSON_COMMAND              = "cmd";
    static final String OEJSON_RESPONSE             = "res";
    static final String OEJSON_ERROR                = "err";
    static final String OEJSON_MESSAGE              = "msg";

    static final String OEJSON_DIS                  = "disconnect";
    static final String OEJSON_CON                  = "connect";
    static final String OEJSON_PUT                  = "put";
    static final String OEJSON_GET                  = "get";
    static final String OEJSON_UPT                  = "utxn";
    static final String OEJSON_UPL                  = "ulease";
    static final String OEJSON_STA                  = "start";

    static final String OEJSON_SID                  = "sid";
    static final String OEJSON_EXTENTNAME           = "extentname";
    static final String OEJSON_USERNAME             = "username";
    static final String OEJSON_PASSWORD             = "password";

    static final String OEJSON_VERSION              = "version";
    static final String OEJSON_MAJ                  = "maj";
    static final String OEJSON_MIN                  = "min";
    static final String OEJSON_SEQ                  = "seq";

    static final String OEJSON_DUR                  = "dur";
    static final String OEJSON_BYTES                = "bytes";
    static final String OEJSON_VALUES               = "values";
    static final String OEJSON_TEMPLATES            = "templates";
    static final String OEJSON_TIMEOUT              = "timeout";
    static final String OEJSON_MAXRESULTS           = "c";
    static final String OEJSON_ISJOIN               = "join";
    static final String OEJSON_IFEXISTS             = "ife";
    static final String OEJSON_REMOVE               = "rm";
    static final String OEJSON_RETURN_ATTRS         = "rattrs";
    static final String OEJSON_RETURN_BYTES         = "rbytes";
    static final String OEJSON_RETURN_IS_DESTROYED  = "rm";
    static final String OEJSON_LID                  = "lid";
    static final String OEJSON_STATUS               = "s";
    static final String OEJSON_ENLISTED             = "c";
    static final String OEJSON_EID                  = "eid";
    static final String OEJSON_NRESULTS             = "c";
    static final String OEJSON_EXPTIME              = "exp";

    private final JSONParser _parser;

    public JsonMessageCoder() {
        _parser = new JSONParser();
    }

    ///////////////////////
    /// begin interface ///
    ///////////////////////

    public Message fromString(String s) throws MessageCoderException {

        if (s == null) {
            throw new java.lang.NullPointerException("string is null");
        }

        Reader in = new StringReader(s); 
        Message msg = null;
        JSONObject obj;
        String type;
        try {
            obj = (JSONObject) _parser.parse(in);
        } catch (Exception e) {
            throw new MessageCoderException(e);
        }
        if (obj == null)
            throw new MessageCoderException("null message parse error");
        if (obj.containsKey(OEJSON_TYPE)) {
            type = (String) obj.get(OEJSON_TYPE);
        } else {
            throw new MessageCoderException("no message type");
        }
        if (type == null)
            throw new MessageCoderException("null message type");
        if (type.equals(OEJSON_COMMAND)) {
            msg = fromCommandString(obj);
        } else if (type.equals(OEJSON_RESPONSE)) {
            msg = fromResponseString(obj);
        } else if (type.equals(OEJSON_ERROR)) {
            msg = fromErrorString(obj);
        } else {
            throw new MessageCoderException("unsupported message type: " + type);
        }
        return msg;
    }
    public Error fromErrorString(JSONObject obj) 
    throws MessageCoderException {
        Error e = new Error();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            e.setCid(new Id(cid.longValue()));
        }
        java.lang.Number eid = (Number) obj.get(OEJSON_EID);
        if (eid != null) {
            e.setId(new Id(eid.longValue()));
        }
        e.setMsg((String) obj.get(OEJSON_MESSAGE));
        return e;
    }
    public Message fromCommandString(JSONObject obj) throws MessageCoderException {
        Message msg = null;
        String name;
        if (obj == null)
            throw new MessageCoderException("null command string");
        if (obj.containsKey(OEJSON_NAME)) {
            name = (String) obj.get(OEJSON_NAME);
        } else {
            throw new MessageCoderException("no command name");
        }
        if (name == null)
            throw new MessageCoderException("null command name");
        if (name.equals(OEJSON_CON)) {
            msg = fromConCommandString(obj);
        } else if (name.equals(OEJSON_DIS)) {
            msg = fromDisCommandString(obj);
        } else if (name.equals(OEJSON_GET)) {
            msg = fromGetCommandString(obj);
        } else if (name.equals(OEJSON_PUT)) {
            msg = fromPutCommandString(obj);
        } else if (name.equals(OEJSON_STA)) {
            msg = fromStaCommandString(obj);
        } else if (name.equals(OEJSON_UPL)) {
            msg = fromUplCommandString(obj);
        } else if (name.equals(OEJSON_UPT)) {
            msg = fromUptCommandString(obj);
        } else {
            throw new MessageCoderException("unsupported command name: " + name);
        }
        return msg;
    }
    public ConCommand fromConCommandString(JSONObject obj) 
    throws MessageCoderException {
        ConCommand c = new ConCommand();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            c.setCid(new Id(cid.longValue()));
        }
        c.setExtentName((String) obj.get(OEJSON_EXTENTNAME));
        c.setUsername((String) obj.get(OEJSON_USERNAME));
        c.setPwd((String) obj.get(OEJSON_PASSWORD));
        JSONObject ver = (JSONObject) obj.get(OEJSON_VERSION);
        java.lang.Number maj = (Number) ver.get(OEJSON_MAJ);
        c.setMaj(maj.intValue());
        java.lang.Number min = (Number) ver.get(OEJSON_MIN);
        c.setMin(min.intValue());
        java.lang.Number upt = (Number) ver.get(OEJSON_SEQ);
        c.setUpt(upt.intValue());
        return c;
    }
    public DisCommand fromDisCommandString(JSONObject obj) 
    throws MessageCoderException {
        return new DisCommand();
    }
    public GetCommand fromGetCommandString(JSONObject obj) 
    throws MessageCoderException {
        GetCommand c = new GetCommand();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            c.setCid(new Id(cid.longValue()));
        }
        java.lang.Number tid = (Number) obj.get(OEJSON_TID);
        if (tid != null) {
            c.setTid(new Id(tid.longValue()));
        }
        java.lang.Number dur = (Number) obj.get(OEJSON_TIMEOUT);
        if (dur != null) {
            c.setTimeoutDur(dur.longValue());
        }
        java.lang.Number mr = (Number) obj.get(OEJSON_MAXRESULTS);
        if (mr != null) {
            c.setMaxResults(mr.intValue());
        }
        Boolean remove = (Boolean) obj.get(OEJSON_REMOVE);
        if (remove != null && remove) {
            c.setIsRemove(true);
        }
        Boolean ifExists = (Boolean) obj.get(OEJSON_IFEXISTS);
        if (ifExists != null && ifExists) {
            c.setIsIfExists(true);
        }
        Boolean returnAttrs = (Boolean) obj.get(OEJSON_RETURN_ATTRS);
        if (returnAttrs != null && returnAttrs) {
            c.setReturnAttrs(true);
        }
        Boolean returnBytes = (Boolean) obj.get(OEJSON_RETURN_BYTES);
        if (returnBytes != null && returnBytes) {
            c.setReturnBytes(true);
        }
        Boolean isJoin = (Boolean) obj.get(OEJSON_ISJOIN);
        if (isJoin != null && isJoin) {
            c.setIsJoin(true);
        }
        //templates is either a list of lists or a list of maps
        List templates = (List) obj.get(OEJSON_TEMPLATES);
        if (templates != null) {
            List<List<String>> t = (List<List<String>>) c.getAttrs();
            List<String> ta = new ArrayList<String>();
            t.add(ta);
            for (Object values : templates) {
                if (values instanceof List) {
                    c.setIsTuple(true);
                    List<String> attrs = (List<String>) values;
                    for (String attr : attrs) {
                        ta.add(attr);
                    }
                } else {
                    Map<String, String> attrs = (Map<String, String>) values;
                    for (Map.Entry<String, String> entry : attrs.entrySet()) {
                        String key = entry.getKey();
                        String value = entry.getValue();
                        ta.add(key);
                        ta.add(value);
                    }
                }
            }
        }

        return c;
    }
    public PutCommand fromPutCommandString(JSONObject obj) 
    throws MessageCoderException {
        PutCommand c = new PutCommand();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            c.setCid(new Id(cid.longValue()));
        }
        java.lang.Number tid = (Number) obj.get(OEJSON_TID);
        if (tid != null) {
            c.setTid(new Id(tid.longValue()));
        }
        java.lang.Number dur = (Number) obj.get(OEJSON_DUR);
        if (dur != null) {
            c.setLeaseDur(dur.longValue());
        }
        String bytesString = (String) obj.get(OEJSON_BYTES);
        if (bytesString != null) {
            try {
                byte[] bytes = Base64.decode(bytesString);
                c.setBytes(bytes);
            } catch (java.io.IOException ioe) {
                throw new MessageCoderException(ioe);
            }
        }
        List<String> ts = (List<String>) c.getAttrs();
        Object values = obj.get(OEJSON_VALUES);
        if (values instanceof List) {
            c.setIsTuple(true);
            List<String> attrs = (List<String>) values;
            if (attrs != null) {
                for (String attr : attrs) {
                    ts.add(attr);
                }
            }
        } else if (values instanceof Map) {
            Map<String, String> attrs = (Map<String, String>) values;
            if (attrs != null) {
                for (Map.Entry<String, String> entry : attrs.entrySet()) {
                    String key = entry.getKey();
                    String value = entry.getValue();
                    ts.add(key);
                    ts.add(value);
                }
            }
        }

        return c;
    }
    public StaCommand fromStaCommandString(JSONObject obj) 
    throws MessageCoderException {
        StaCommand c = new StaCommand();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            c.setCid(new Id(cid.longValue()));
        }
        java.lang.Number dur = (Number) obj.get(OEJSON_DUR);
        if (dur != null) {
            c.setLeaseDur(dur.longValue());
        }
        return c;
    }
    public UplCommand fromUplCommandString(JSONObject obj) 
    throws MessageCoderException {
        UplCommand c = new UplCommand();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            c.setCid(new Id(cid.longValue()));
        }
        java.lang.Number dur = (Number) obj.get(OEJSON_DUR);
        if (dur != null) {
            c.setLeaseDur(dur.longValue());
        }
        java.lang.Number lid = (Number) obj.get(OEJSON_LID);
        if (lid != null) {
            c.setCid(new Id(lid.longValue()));
        }
        return c;
    }
    public UptCommand fromUptCommandString(JSONObject obj) 
    throws MessageCoderException {
        UptCommand c = new UptCommand();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            c.setCid(new Id(cid.longValue()));
        }
        java.lang.Number tid = (Number) obj.get(OEJSON_TID);
        if (tid != null) {
            c.setCid(new Id(tid.longValue()));
        }
        java.lang.Number dur = (Number) obj.get(OEJSON_DUR);
        if (dur != null) {
            c.setLeaseDur(dur.longValue());
        }
        java.lang.Number enlisted = (Number) obj.get(OEJSON_ENLISTED);
        if (enlisted != null) {
            c.setEnlisted(enlisted.intValue());
        }
        String status_str = (String) obj.get(OEJSON_STATUS);
        if (status_str != null) {
            if ( status_str.equals("A") ) {
                c.setStatus(TxnStatus.ACTIVE);
            } else if ( status_str.equals("C") ) {
                c.setStatus(TxnStatus.COMMITTED);
            } else if ( status_str.equals("R") ) {
                c.setStatus(TxnStatus.ROLLEDBACK);
            } else {
                throw new java.lang.IllegalArgumentException("bad txn status");
            }
        }
        return c;
    }

    public Message fromResponseString(JSONObject obj) throws MessageCoderException {
        Message msg = null;
        String name;
        if (obj == null)
            throw new MessageCoderException("null response string");
        if (obj.containsKey(OEJSON_NAME)) {
            name = (String) obj.get(OEJSON_NAME);
        } else {
            throw new MessageCoderException("no response name");
        }
        if (name == null)
            throw new MessageCoderException("null response name");
        if (name.equals(OEJSON_CON)) {
            msg = fromConResponseString(obj);
        } else if (name.equals(OEJSON_GET)) {
            msg = fromGetResponseString(obj);
        } else if (name.equals(OEJSON_PUT)) {
            msg = fromPutResponseString(obj);
        } else if (name.equals(OEJSON_STA)) {
            msg = fromStaResponseString(obj);
        } else if (name.equals(OEJSON_UPL)) {
            msg = fromUplResponseString(obj);
        } else if (name.equals(OEJSON_UPT)) {
            msg = fromUptResponseString(obj);
        } else {
            throw new MessageCoderException("unsupported response name: " + name);
        }
        return msg;
    }
    public ConResponse fromConResponseString(JSONObject obj) 
    throws MessageCoderException {
        ConResponse r = new ConResponse();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            r.setCid(new Id(cid.longValue()));
        }
        java.lang.Number sid = (Number) obj.get(OEJSON_SID);
        if (sid != null) {
            r.setSid(new Id(sid.longValue()));
        }
        JSONObject ver = (JSONObject) obj.get(OEJSON_VERSION);
        java.lang.Number maj = (Number) ver.get(OEJSON_MAJ);
        r.setMaj(maj.intValue());
        java.lang.Number min = (Number) ver.get(OEJSON_MIN);
        r.setMin(min.intValue());
        java.lang.Number upt = (Number) ver.get(OEJSON_SEQ);
        r.setUpt(upt.intValue());
        return r;
    }
    public GetResponse fromGetResponseString(JSONObject obj) 
    throws MessageCoderException {
        GetResponse r = new GetResponse();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            r.setCid(new Id(cid.longValue()));
        }
        java.lang.Number tid = (Number) obj.get(OEJSON_TID);
        if (tid != null) {
            r.setTid(new Id(tid.longValue()));
        }
        java.lang.Number nresults = (Number) obj.get(OEJSON_NRESULTS);
        if (nresults != null) {
            r.setNResults(nresults.intValue());
        }
        String bytesString = (String) obj.get(OEJSON_BYTES);
        if (bytesString != null) {
            try {
                byte[] bytes = Base64.decode(bytesString);
                r.setBytes(bytes);
            } catch (java.io.IOException ioe) {
                throw new MessageCoderException(ioe);
            }
        }

        List<String> ts = (List<String>) r.getAttrs();
        Object values = obj.get(OEJSON_VALUES);
        if (values instanceof List) {
            List<String> attrs = (List<String>) values;
            for (String attr : attrs) {
                ts.add(attr);
            }
        } else if (values instanceof Map) {
            Map<String, String> attrs = (Map<String, String>) values;
            for (Map.Entry<String, String> entry : attrs.entrySet()) {
                String key = entry.getKey();
                String value = entry.getValue();
                ts.add(key);
                ts.add(value);
            }
        }
        return r;
    }
    public PutResponse fromPutResponseString(JSONObject obj) 
    throws MessageCoderException {
        PutResponse r = new PutResponse();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            r.setCid(new Id(cid.longValue()));
        }
        java.lang.Number tid = (Number) obj.get(OEJSON_TID);
        if (tid != null) {
            r.setTid(new Id(tid.longValue()));
        }
        java.lang.Number lid = (Number) obj.get(OEJSON_LID);
        if (lid != null) {
            r.setLeaseId(new Id(lid.longValue()));
        }
        java.lang.Number exptime = (Number) obj.get(OEJSON_EXPTIME);
        if (exptime != null) {
            r.setExptime(exptime.longValue());
        }
        return r;
    }
    public StaResponse fromStaResponseString(JSONObject obj) 
    throws MessageCoderException {
        StaResponse r = new StaResponse();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            r.setCid(new Id(cid.longValue()));
        }
        java.lang.Number tid = (Number) obj.get(OEJSON_TID);
        if (tid != null) {
            r.setTid(new Id(tid.longValue()));
        }
        java.lang.Number exptime = (Number) obj.get(OEJSON_EXPTIME);
        if (exptime != null) {
            r.setExptime(exptime.longValue());
        }
        return r;
    }
    public UplResponse fromUplResponseString(JSONObject obj) 
    throws MessageCoderException {
        UplResponse r = new UplResponse();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            r.setCid(new Id(cid.longValue()));
        }
        java.lang.Number exptime = (Number) obj.get(OEJSON_EXPTIME);
        if (exptime != null) {
            r.setExptime(exptime.longValue());
        }
        return r;
    }
    public UptResponse fromUptResponseString(JSONObject obj) 
    throws MessageCoderException {
        UptResponse r = new UptResponse();
        java.lang.Number cid = (Number) obj.get(OEJSON_CID);
        if (cid != null) {
            r.setCid(new Id(cid.longValue()));
        }
        /*
        java.lang.Number exptime = (Number) obj.get(OEJSON_EXPTIME);
        if (exptime != null) {
            r.setExptime(exptime.longValue());
        }
        */
        String status_str = (String) obj.get(OEJSON_STATUS);
        if (status_str != null) {
            if ( status_str.equals("A") ) {
                r.setStatus(TxnStatus.ACTIVE);
            } else if ( status_str.equals("C") ) {
                r.setStatus(TxnStatus.COMMITTED);
            } else if ( status_str.equals("R") ) {
                r.setStatus(TxnStatus.ROLLEDBACK);
            } else {
                throw new java.lang.IllegalArgumentException("bad txn status");
            }
        }
        return r;
    }

    public String toString(Message m) throws MessageCoderException {

        if (m == null) {
            throw new java.lang.NullPointerException("Message is null");
        }

        String s = null;
        if (m instanceof Command) {

            s = commandToString((Command) m);

        } else if (m instanceof Response) {

            s = responseToString((Response) m);

        } else {
            throw new java.lang.IllegalArgumentException(
                                                        "invalid message type: " + m.getClass().toString());
        }
        return s;
    }

    //////////////////
    /// begin impl ///
    //////////////////

    private static String commandToString(Command c) {
        if (c == null) {
            throw new java.lang.NullPointerException("Command is null");
        }

        String s = null;
        if (c instanceof ConCommand) {
            s = conCommandToString((ConCommand) c);

        } else if (c instanceof DisCommand) {
            s = disCommandToString((DisCommand) c);

        } else if (c instanceof GetCommand) {
            s = getCommandToString((GetCommand) c);

        } else if (c instanceof PutCommand) {
            s = putCommandToString((PutCommand) c);

        } else if (c instanceof StaCommand) {
            s = staCommandToString((StaCommand) c);

        } else if (c instanceof UplCommand) {
            s = uplCommandToString((UplCommand) c);

        } else if (c instanceof UptCommand) {
            s = uptCommandToString((UptCommand) c);

        } else {
            throw new java.lang.IllegalArgumentException(
                                                        "invalid command type: " + c.getClass().toString());
        }
        return s;
    }

    private static String conCommandToString(ConCommand c) {

        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_COMMAND);
        obj.put(OEJSON_NAME, OEJSON_CON);
        obj.put(OEJSON_EXTENTNAME, c.getExtentName());
        obj.put(OEJSON_USERNAME, c.getUsername());
        obj.put(OEJSON_PASSWORD, c.getPassword());
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        JSONObject ver = new JSONObject();
        obj.put(OEJSON_VERSION, ver);
        ver.put(OEJSON_MAJ, c.getMaj());
        ver.put(OEJSON_MIN, c.getMin());
        ver.put(OEJSON_SEQ, c.getUpt());

        return obj.toString();
    }
    private static String disCommandToString(DisCommand c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_COMMAND);
        obj.put(OEJSON_NAME, OEJSON_DIS);

        return obj.toString();
    }
    private static String getCommandToString(GetCommand c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_COMMAND);
        obj.put(OEJSON_NAME, OEJSON_GET);
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        if (c.getTid() != Message.NILID)
            obj.put(OEJSON_TID, c.getTid().getLong());
        obj.put(OEJSON_TIMEOUT, c.getTimeoutDur());
        obj.put(OEJSON_MAXRESULTS, c.getMaxResults());
        if (c.isRemove())
            obj.put(OEJSON_REMOVE, true);
        if (c.isIfExists())
            obj.put(OEJSON_IFEXISTS, true);
        if (c.returnAttrs())
            obj.put(OEJSON_RETURN_ATTRS, true);
        if (c.returnBytes())
            obj.put(OEJSON_RETURN_BYTES, true);
        if (c.isJoin())
            obj.put(OEJSON_ISJOIN, true);
        JSONArray templates = new JSONArray();
        obj.put(OEJSON_TEMPLATES, templates);
        boolean isTuple = c.isTuple();
        for (List<String> t: c.getAttrs()) {

            if (isTuple) {
                JSONArray template = new JSONArray();
                templates.add(template);
                for (String attr: t) {
                    if (attr == null) {
                        template.add("_");
                    } else {
                        template.add(attr);
                    }
                }
            } else {
                JSONObject template = new JSONObject();
                templates.add(template);
                for (Iterator<String> iter = t.iterator(); iter.hasNext();) {
                    String key = iter.next();
                    String value = iter.next();
                    template.put(key, value);
                }
            }
        }
        return obj.toString();
    }
    private static String putCommandToString(PutCommand c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_COMMAND);
        obj.put(OEJSON_NAME, OEJSON_PUT);
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        if (c.getTid() != Message.NILID)
            obj.put(OEJSON_TID, c.getTid().getLong());
        obj.put(OEJSON_DUR, c.getLeaseDur());
        if (c.hasBytes()) {
            obj.put(OEJSON_BYTES, Base64.encodeBytes(c.getBytes()));
        }
        if (c.isTuple()) {
            JSONArray attrs = new JSONArray();
            obj.put(OEJSON_VALUES, attrs);
            for (String attr: c.getAttrs()) {
                if (attr == null) {
                    attrs.add("_");
                } else {
                    attrs.add(attr);
                }
            }
        } else {
            JSONObject attrs = new JSONObject();
            obj.put(OEJSON_VALUES, attrs);
            for (Iterator iter = c.getAttrs().iterator(); iter.hasNext();) {
                String key = (String) iter.next();
                if (key == null) key = "_";
                String value = (String) iter.next();
                if (value == null) value = "_";
                attrs.put(key, value);
            }
        }
        return obj.toString();
    }
    private static String staCommandToString(StaCommand c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_COMMAND);
        obj.put(OEJSON_NAME, OEJSON_STA);
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        obj.put(OEJSON_DUR, c.getLeaseDur());
        return obj.toString();
    }
    private static String uplCommandToString(UplCommand c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_COMMAND);
        obj.put(OEJSON_NAME, OEJSON_UPL);
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        obj.put(OEJSON_DUR, c.getLeaseDur());
        obj.put(OEJSON_LID, c.getLeaseId().getLong());
        return obj.toString();
    }
    private static String uptCommandToString(UptCommand c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_COMMAND);
        obj.put(OEJSON_NAME, OEJSON_UPT);
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        if (c.getTid() != Message.NILID)
            obj.put(OEJSON_TID, c.getTid().getLong());
        obj.put(OEJSON_DUR, c.getLeaseDur());
        obj.put(OEJSON_ENLISTED, c.getEnlisted());
        TxnStatus status = c.getStatus();
        switch (status) {
        case ACTIVE: obj.put(OEJSON_STATUS, "A"); break;
        case COMMITTED: obj.put(OEJSON_STATUS, "C"); break;
        case ROLLEDBACK: obj.put(OEJSON_STATUS, "R"); break;
        default: throw new java.lang.IllegalArgumentException("bad txn status");
        }
        return obj.toString();
    }


    private static String responseToString(Response r) {
        if (r == null) {
            throw new java.lang.NullPointerException("Response is null");
        }

        String s = null;
        if (r instanceof ConResponse) {
            s = conResponseToString((ConResponse) r);

        } else if (r instanceof GetResponse) {
            s = getResponseToString((GetResponse) r);

        } else if (r instanceof PutResponse) {
            s = putResponseToString((PutResponse) r);

        } else if (r instanceof StaResponse) {
            s = staResponseToString((StaResponse) r);

        } else if (r instanceof UplResponse) {
            s = uplResponseToString((UplResponse) r);

        } else if (r instanceof UptResponse) {
            s = uptResponseToString((UptResponse) r);

        } else {
            throw new java.lang.IllegalArgumentException(
                                                        "invalid response type: " + r.getClass().toString());
        }
        return s;
    }

    private static String conResponseToString(ConResponse c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_RESPONSE);
        obj.put(OEJSON_NAME, OEJSON_CON);
        obj.put(OEJSON_SID, c.getSid().getLong());
        if (c.getCid() != Message.NILID) {
            obj.put(OEJSON_CID, c.getCid().getLong());
        }
        JSONObject ver = new JSONObject();
        obj.put(OEJSON_VERSION, ver);
        ver.put(OEJSON_MAJ, c.getMaj());
        ver.put(OEJSON_MIN, c.getMin());
        ver.put(OEJSON_SEQ, c.getUpt());

        return obj.toString();
    }
    private static String getResponseToString(GetResponse c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_RESPONSE);
        obj.put(OEJSON_NAME, OEJSON_GET);
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        if (c.getTid() != Message.NILID)
            obj.put(OEJSON_TID, c.getTid().getLong());
        obj.put(OEJSON_NRESULTS, c.getNResults());
        if (c.hasBytes()) {
            obj.put(OEJSON_BYTES, Base64.encodeBytes(c.getBytes()));
        }

        if (c.isTuple()) {
            JSONArray attrs = new JSONArray();
            obj.put(OEJSON_VALUES, attrs);
            for (String attr: c.getAttrs()) {
                if (attr == null) {
                    attrs.add("_");
                } else {
                    attrs.add(attr);
                }
            }
        } else {
            JSONObject attrs = new JSONObject();
            obj.put(OEJSON_VALUES, attrs);
            for (Iterator<String> iter = c.getAttrs().iterator(); iter.hasNext();) {
                String key = iter.next();
                String value = iter.next();
                attrs.put(key, value);
            }
        }
        return obj.toString();
    }
    private static String putResponseToString(PutResponse c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_RESPONSE);
        obj.put(OEJSON_NAME, OEJSON_PUT);
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        if (c.getTid() != Message.NILID)
            obj.put(OEJSON_TID, c.getTid().getLong());
        obj.put(OEJSON_EXPTIME, c.getExptime());
        return obj.toString();
    }
    private static String staResponseToString(StaResponse c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_RESPONSE);
        obj.put(OEJSON_NAME, OEJSON_STA);
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        if (c.getTid() != Message.NILID)
            obj.put(OEJSON_TID, c.getTid().getLong());
        obj.put(OEJSON_EXPTIME, c.getExptime());
        return obj.toString();
    }
    private static String uplResponseToString(UplResponse c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_RESPONSE);
        obj.put(OEJSON_NAME, OEJSON_UPL);
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        obj.put(OEJSON_EXPTIME, c.getExptime());
        return obj.toString();
    }
    private static String uptResponseToString(UptResponse c) {
        JSONObject obj = new JSONObject();
        obj.put(OEJSON_TYPE, OEJSON_RESPONSE);
        obj.put(OEJSON_NAME, OEJSON_UPT);
        if (c.getCid() != Message.NILID)
            obj.put(OEJSON_CID, c.getCid().getLong());
        //obj.put(OEJSON_EXPTIME, c.getExptime());
        TxnStatus status = c.getStatus();
        switch (status) {
        case ACTIVE: obj.put(OEJSON_STATUS, "A"); break;
        case COMMITTED: obj.put(OEJSON_STATUS, "C"); break;
        case ROLLEDBACK: obj.put(OEJSON_STATUS, "R"); break;
        default: throw new java.lang.IllegalArgumentException("bad txn status");
        }
        return obj.toString();
    }

    private static void appendTabs(StringBuffer sb, int c) {
        for (int i = 0; i < (c); i++) {
            sb.append(' ');
            sb.append(' ');
            sb.append(' ');
            sb.append(' ');
        }
    }
    public static String prettyPrint(String s) {
        //this is nuts, todo: redo with parser
        int level = 0;
        boolean datamode=false;
        boolean escmode=false;
        StringBuffer sb=new StringBuffer();
        for (int i=0;i<s.length();i++) {
            char ch=s.charAt(i);
            if (escmode) {
                sb.append(ch);
                escmode = false;
                continue;
            }
            if (datamode && ch != '"') {
                sb.append(ch);
                continue;
            }
            switch (ch) {
            case '\\':
                sb.append(ch);
                escmode = true;
                break;
            case '"':
                sb.append(ch);
                datamode = !datamode;
                break;
            case '{':
                sb.append("{\n");
                appendTabs(sb, ++level);
                break;
            case '}':
                sb.append("\n");
                appendTabs(sb, --level);
                sb.append("}\n");
                appendTabs(sb, level);
                break;
            case '[':
                sb.append("[\n");
                appendTabs(sb, ++level);
                break;
            case ']':
                sb.append("\n");
                appendTabs(sb, --level);
                sb.append("]\n");
                appendTabs(sb, level);
                break;
            case ',':
                sb.append(",\n");
                appendTabs(sb, level);
                break;
            default:
                sb.append(ch);
            }
        }
        return sb.toString().trim();
    }
}

