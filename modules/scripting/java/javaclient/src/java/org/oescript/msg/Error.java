package org.oescript.msg;
import org.oescript.api.Id;

public class Error extends Message {

    private String  _msg;
    private Id      _eid;

    public Error setMsg(String msg) {
        _msg = msg;
        return this;
    }
    public String getMsg() {
        return _msg;
    }
    public Error setId(Id eid) {
        _eid = eid;
        return this;
    }
    public Id getId() {
        return _eid;
    }
}

