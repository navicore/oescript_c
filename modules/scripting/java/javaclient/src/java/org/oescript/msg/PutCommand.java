package org.oescript.msg;
import java.util.ArrayList;
import java.util.List;

public class PutCommand extends Command {

    private long    _lease;
    private boolean _is_tuple;
    private byte[]  _bytes;

    private final List<String> _attrs;

    public PutCommand() {
        _lease          = 60 * 1000 * 10; //10 minutes
        _is_tuple       = false;
        _attrs          = new ArrayList<String>();
    }

   
    public PutCommand setLeaseDur(long dur) {
        _lease = dur;
        return this;
    }
    public long getLeaseDur() {
        return _lease;
    }
    public PutCommand setIsTuple(boolean b) {
        _is_tuple = b;
        return this;
    }
    public boolean isTuple() {
        return _is_tuple;
    }
    public List<String> getAttrs() {
        return _attrs;
    }
    public PutCommand setBytes(byte[] bytes) {
        _bytes = bytes;
        return this;
    }
    public byte[] getBytes() {
        return _bytes;
    }
    public boolean hasBytes() {
        return _bytes != null;
    }
}

