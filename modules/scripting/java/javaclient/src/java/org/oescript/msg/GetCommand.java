package org.oescript.msg;
import java.util.ArrayList;
import java.util.List;

public class GetCommand extends Command {

    private long _timeout;
    private int _max_results;
    private boolean _is_tuple, _is_join, _return_bytes,
        _return_attrs, _remove, _if_exists;
    private final List<List<String>> _attrs;

    public GetCommand() {
        _max_results    = 1;
        _timeout        = 30 * 1000; //30 secs
        _is_tuple       = true;
        _is_join        = false;
        _return_attrs   = true;
        _return_bytes   = true;
        _remove         = false;
        _if_exists      = false;
        _attrs          = new ArrayList<List<String>>();
    }

   
    public GetCommand setTimeoutDur(long dur) {
        _timeout = dur;
        return this;
    }
    public long getTimeoutDur() {
        return _timeout;
    }
    public GetCommand setMaxResults(int max) {
        _max_results = max;
        return this;
    }
    public long getMaxResults() {
        return _max_results;
    }
    public GetCommand setIsTuple(boolean b) {
        _is_tuple = b;
        return this;
    }
    public boolean isTuple() {
        return _is_tuple;
    }
    public GetCommand setIsJoin(boolean b) {
        _is_join = b;
        return this;
    }
    public boolean isJoin() {
        return _is_join;
    }
    public GetCommand setReturnBytes(boolean b) {
        _return_bytes = b;
        return this;
    }
    public boolean returnBytes() {
        return _return_bytes;
    }
    public GetCommand setReturnAttrs(boolean b) {
        _return_attrs = b;
        return this;
    }
    public boolean returnAttrs() {
        return _return_attrs;
    }
    public GetCommand setIsRemove(boolean b) {
        _remove = b;
        return this;
    }
    public boolean isRemove() {
        return _remove;
    }
    public GetCommand setIsIfExists(boolean b) {
        _if_exists = b;
        return this;
    }
    public boolean isIfExists() {
        return _if_exists;
    }
    public List<List<String>> getAttrs() {
        return _attrs;
    }
}

