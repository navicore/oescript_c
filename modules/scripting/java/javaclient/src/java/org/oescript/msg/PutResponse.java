package org.oescript.msg;
import java.util.ArrayList;
import java.util.List;
import org.oescript.api.Id;

public class PutResponse extends Response {

    private long    _exptime; //todo: datetime
    private Id      _lid;

    public PutResponse setExptime(long time) {
        _exptime = time;
        return this;
    }
    public long getExptime() {
        return _exptime;
    }
    public PutResponse setLeaseId(Id lid) {
        _lid = lid;
        return this;
    }
    public Id getLeaseId() {
        return _lid;
    }
}


