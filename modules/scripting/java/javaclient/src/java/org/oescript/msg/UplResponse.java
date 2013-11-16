package org.oescript.msg;
import java.util.ArrayList;
import java.util.List;

public class UplResponse extends Response {

    private long    _exptime; //todo: datetime

    public UplResponse setExptime(long time) {
        _exptime = time;
        return this;
    }
    public long getExptime() {
        return _exptime;
    }
}

