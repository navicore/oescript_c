package org.oescript.msg;
import java.util.ArrayList;
import java.util.List;

public class StaResponse extends Response {

    private long    _exptime;

    public StaResponse setExptime(long time) {
        _exptime = time;
        return this;
    }
    public long getExptime() {
        return _exptime;
    }
}

