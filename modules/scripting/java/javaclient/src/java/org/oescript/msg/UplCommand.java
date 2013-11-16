package org.oescript.msg;
import java.util.ArrayList;
import java.util.List;

import org.oescript.api.Id;

public class UplCommand extends Command {

    private long    _lease;
    private Id      _lid;

    public UplCommand setLeaseDur(long dur) {
        _lease = dur;
        return this;
    }
    public long getLeaseDur() {
        return _lease;
    }
    public UplCommand setLeaseId(Id lid) {
        _lid = lid;
        return this;
    }
    public Id getLeaseId() {
        return _lid;
    }
}

