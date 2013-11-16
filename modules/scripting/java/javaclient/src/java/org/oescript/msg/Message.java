package org.oescript.msg;
import org.oescript.api.Id;

public class Message {

    private Id _tid, _cid;
    public static final Id NILID = new Id(0);
    public static final String WILDCARD = "_";
    public static final String NULL = "_";

    Message() {
        _tid = NILID;
        _cid = NILID;
    }

    public Message setCid(Id cid) {
        _cid = cid;
        return this;
    }
    public Id getCid() {
        return _cid;
    }
    public Message setTid(Id tid) {
        _tid = tid;
        return this;
    }
    public Id getTid() {
        return _tid;
    }
    public String toString() {
        return "OeScript Message: " + this.getClass().getName() + " cid: " + _cid;
    }
}

