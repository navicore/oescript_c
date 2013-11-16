package org.oescript.msg;
import java.util.ArrayList;
import java.util.List;
import org.oescript.api.TxnStatus;

public class UptResponse extends Response {

    //private long    _exptime;
    private TxnStatus   _status;

    /*
    public UptResponse setExptime(long time) {
        _exptime = time;
        return this;
    }
    public long getExptime() {
        return _exptime;
    }
    */
    public UptResponse setStatus(TxnStatus s) {
        _status = s;
        return this;
    }
    public TxnStatus getStatus() {
        return _status;
    }
}

