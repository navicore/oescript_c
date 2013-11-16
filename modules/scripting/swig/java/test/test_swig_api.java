import org.oescript.java.swig.*;

public class test_swig_api {
    static {
        System.loadLibrary("oescriptimpl");
    }

    public static void main(String argv[]) {

        OesSigHandler signals = null;
        assert(signals == null);
        signals = new OesSigHandler("mysigfile.txt", "oesjava", "info");
        assert(signals != null);

        OesDispatcher dispatcher = new OesDispatcher(1, "mydispatcher");
        assert(dispatcher != null);
        dispatcher.start();

        OesThreadDispatcher tdispatcher = new OesThreadDispatcher(4);
        assert(tdispatcher != null);
        tdispatcher.start();

        OesStore store = new OesStore(3, "data", true);
        assert(store != null);

        OesKernel kernel = new OesKernel(dispatcher, tdispatcher, store, 
                                         null, null, null, null);
        assert(kernel != null);

        //start socket server
        OesNet net = new OesNet(dispatcher, false, null, null);
        OesServer json = new OesServer(dispatcher, "oejson://0.0.0.0:7777", net, store);
        json.addKernel("myspace", kernel);

	kernel.delete();
	store.delete();
    }
}

