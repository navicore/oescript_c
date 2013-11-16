import org.oescript.msg.*;
import java.util.List;
import java.util.ArrayList;
import org.oescript.api.Id;
import org.oescript.api.TxnStatus;

public class test_msgs {

    static IdFactory _id_factory = new IdFactory();
    static MessageCoder _coder = new JsonMessageCoder();

    public static void main(String[] args) throws Exception {

        assert(true);
        //assert(false);

        boolean gotone = false;
        try {
            _coder.toString(null);
        } catch (NullPointerException e) {
            gotone = true; //yay
        }
        assert(gotone);
        test_id();
        test_con();
        test_get();
        test_put();
        test_sta();
        test_upl();
        test_upt();
    }

    private static void test_id() {
        Id id1 = _id_factory.nextId();
        Id id2 = _id_factory.nextId();
        assert(!id1.equals(id2));
        assert(id1.equals(id1));
        System.out.println(id1);
        System.out.println(id2);
    }
    private static void test_con() throws Exception {
        //create con command
        Id cid = _id_factory.nextId();
        ConCommand c = new ConCommand();
        c.setCid(cid);
        c.setMaj(0);
        c.setMin(4);
        c.setUpt(2);
        c.setExtentName("myspace");
        c.setUsername("myname");

        String s = _coder.toString(c);
        assert(s != null);
        System.out.println(s);

        //create con response
        ConResponse r = new ConResponse();
        r.setCid(cid);
        Id sid = _id_factory.nextId();
        r.setSid(sid);
        r.setMaj(0);
        r.setMin(4);
        r.setUpt(1);

        String rs = _coder.toString(r);
        assert(rs != null);
        System.out.println(rs);

        //recreate con command
        ConCommand nc = (ConCommand) _coder.fromString(s);
        assert(nc != null);
        assert(nc.getCid().equals(cid));

        //recreate con response
        System.out.println("");
    }
    private static void test_get() throws Exception {
        //create get command
        Id cid = _id_factory.nextId();
        Id tid = _id_factory.nextId();
        GetCommand c = new GetCommand();
        c.setIsTuple(true);
        c.setCid(cid);
        c.setTid(tid);
        List attrs = new ArrayList();
        attrs.add("one");
        attrs.add("two");
        attrs.add("three");
        c.getAttrs().add(attrs);
        List attrs2 = new ArrayList();
        attrs2.add("1");
        attrs2.add("2");
        attrs2.add("3");
        c.getAttrs().add(attrs2);

        String s = _coder.toString(c);
        assert(s != null);
        System.out.println(s);

        //create get response
        GetResponse r = new GetResponse();
        r.setCid(cid);
        r.setTid(tid);
        List attrs3 = r.getAttrs();
        attrs3.add("1");
        attrs3.add("2");
        attrs3.add("3");
        r.setBytes("see, i am bytes".getBytes("UTF8"));
        r.setNResults(1);

        String rs = _coder.toString(r);
        assert(rs != null);
        System.out.println(rs);

        //recreate get command
        GetCommand nc = (GetCommand) _coder.fromString(s);
        assert(nc != null);
        assert(nc.getCid().equals(cid));
        assert(nc.getTid().equals(tid));
        String ncs = _coder.toString(nc);
        assert(ncs != null);
        System.out.println("does this look ok\n?" + ncs);
        System.out.println("and this?\n" + JsonMessageCoder.prettyPrint(ncs));

        //recreate get response
        GetResponse nr = (GetResponse) _coder.fromString(rs);
        String rbytes = new String(nr.getBytes(), "UTF8");
        System.out.println("rbytes should be 'see, i am bytes': " + rbytes);
    }
    private static void test_put() throws Exception {
        //create command
        Id cid = _id_factory.nextId();
        Id tid = _id_factory.nextId();
        PutCommand c = new PutCommand();
        c.setIsTuple(true);
        c.setCid(cid);
        c.setTid(tid);
        List<String> attrs = c.getAttrs();
        attrs.add("one1");
        attrs.add("two2");
        attrs.add("three3");

        c.setBytes("i am bytes".getBytes("UTF8"));

        String s = _coder.toString(c);
        assert(s != null);
        System.out.println(s);

        //create response
        Id lid = _id_factory.nextId();
        PutResponse r = new PutResponse();
        r.setCid(cid);
        r.setTid(tid);
        r.setLeaseId(lid);
        r.setExptime(System.currentTimeMillis() + 10000);

        String rs = _coder.toString(r);
        assert(rs != null);
        System.out.println(rs);

        //recreate command
        PutCommand nc = (PutCommand) _coder.fromString(s);
        assert(nc != null);
        assert(nc.getCid().equals(cid));
        String ncs = _coder.toString(nc);
        assert(ncs != null);
        System.out.println("does this look ok? " + ncs);
        //recreate response
        System.out.println("");
    }
    private static void test_sta() throws Exception {
        //create command
        Id cid = _id_factory.nextId();
        StaCommand c = new StaCommand();
        c.setCid(cid);
        c.setLeaseDur(2002);

        String s = _coder.toString(c);
        assert(s != null);
        System.out.println(s);

        //create response
        Id tid = _id_factory.nextId();
        StaResponse r = new StaResponse();
        r.setCid(cid);
        r.setTid(tid);
        r.setExptime(System.currentTimeMillis() + 10000);

        String rs = _coder.toString(r);
        assert(rs != null);
        System.out.println(rs);

        //recreate command

        //recreate response
        System.out.println("");
    }
    private static void test_upl() throws Exception {
        //create command
        Id cid = _id_factory.nextId();
        Id lid = _id_factory.nextId();
        UplCommand c = new UplCommand();
        c.setCid(cid);
        c.setLeaseDur(1001);
        c.setLeaseId(lid);

        String s = _coder.toString(c);
        assert(s != null);
        System.out.println(s);

        //create response
        Id tid = _id_factory.nextId();
        UplResponse r = new UplResponse();
        r.setCid(cid);
        r.setExptime(System.currentTimeMillis() + 10000);

        String rs = _coder.toString(r);
        assert(rs != null);
        System.out.println(rs);

        //recreate command
        //recreate response
        System.out.println("");
    }
    private static void test_upt() throws Exception {
        //create command
        Id cid = _id_factory.nextId();
        Id tid = _id_factory.nextId();
        UptCommand c = new UptCommand();
        c.setCid(cid);
        c.setTid(tid);
        c.setLeaseDur(3003);
        c.setStatus(TxnStatus.COMMITTED);

        String s = _coder.toString(c);
        assert(s != null);
        System.out.println(s);

        //create response
        UptResponse r = new UptResponse();
        r.setCid(cid);
        r.setTid(tid);
        //r.setExptime(System.currentTimeMillis() + 10000);
        r.setStatus(TxnStatus.ROLLEDBACK);

        String rs = _coder.toString(r);
        assert(rs != null);
        System.out.println(rs);

        //recreate command
        //recreate response
        System.out.println("");
    }
}

