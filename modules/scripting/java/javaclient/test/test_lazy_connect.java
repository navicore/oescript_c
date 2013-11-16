import java.io.IOException;
import java.util.*;

import org.oescript.api.*;
import org.oescript.java.swig.*;
import org.oescript.javaclient.ExtentClient;
import org.oescript.javaclient.SocketClient;
import org.oescript.util.StringParse;

public class test_lazy_connect {

    static String SERVER_STR = "oejson://0.0.0.0:6788/someplace";
    static String SADDR_STR  = "127.0.0.1";
    static int    SPORT      = 6788;

    static {
        System.loadLibrary("oescriptimpl");
    }

    public static void main(String[] args) throws Exception {

        System.out.println("json server test starting...");
        startServer();

        test_put_list();
        test_bad_get_list();

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
        s.addKernel("someplace", _kernel);
    }

    static void stopServer() {
        _dispatcher.delete();
        _tdispatcher.delete();
        _kernel.delete();
        _store.delete();
    }
    static void test_put_list() throws IOException {
        ExtentClient ec = new ExtentClient(SERVER_STR, "myname", "mysecret", true);
        ec.connect();
        List l = new ArrayList();
        l.add("lazyone");
        l.add("lazytwo");
        l.add("lazythree");
        Lease lease = ec.writeList(l);
        assert(lease != null);
        assert(lease.getId() != null);
        ec.disconnect();
    }
    static void test_bad_get_list() throws IOException {
        ExtentClient ec = new ExtentClient(SERVER_STR, "myname", "mysecret", true);
        ec.connect();
        List t = new ArrayList();
        t.add("lazyfour");
        t.add(null);
        t.add(null);
        long to = ec.getTimeout();
        ec.setTimeout(0);
        List rl = ec.readList(t);
        ec.setTimeout(to);
        assert(rl == null);
        ec.disconnect();
    }
}

