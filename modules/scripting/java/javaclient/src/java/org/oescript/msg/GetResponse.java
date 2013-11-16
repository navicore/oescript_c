package org.oescript.msg;
import java.util.ArrayList;
import java.util.List;

public class GetResponse extends Response {

    private int _nresults;
    private byte[] _bytes;
    private final List<String> _attrs;
    private boolean _is_tuple;

    public GetResponse() {
        _attrs = new ArrayList<String>();
        _is_tuple       = true;
    }

    public GetResponse setNResults(int n) {
        _nresults = n;
        return this;
    }
    public int getNResults() {
        return _nresults;
    }
    public List<String> getAttrs() {
        return _attrs;
    }
    public GetResponse setBytes(byte[] bytes) {
        _bytes = bytes;
        return this;
    }
    public byte[] getBytes() {
        return _bytes;
    }
    public boolean hasBytes() {
        return _bytes != null;
    }

    public GetResponse setIsTuple(boolean b) {
        _is_tuple = b;
        return this;
    }
    public boolean isTuple() {
        return _is_tuple;
    }
} 

