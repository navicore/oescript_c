package org.oescript.msg;
import java.util.ArrayList;
import java.util.List;

import org.oescript.api.TxnStatus;

public class UptCommand extends Command {

    private long        _lease;
    private int         _enlisted;
    private TxnStatus   _status;

    public UptCommand setEnlisted(int enlisted) {
        _enlisted = enlisted;
        return this;
    }
    public long getEnlisted() {
        return _enlisted;
    }
    public UptCommand setLeaseDur(long dur) {
        _lease = dur;
        return this;
    }
    public long getLeaseDur() {
        return _lease;
    }
    public UptCommand setStatus(TxnStatus s) {
        _status = s;
        return this;
    }
    public TxnStatus getStatus() {
        return _status;
    }
}

