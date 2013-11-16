package org.oescript.msg;
import org.oescript.api.Id;

public class ConResponse extends Response {

    private int _maj, _min, _upt;
    private Id _sid;
    private String _extname;

    public ConResponse setMaj(int maj) {
        _maj = maj;
        return this;
    }
    public int getMaj() {
        return _maj;
    }
    public ConResponse setMin(int min) {
        _min = min;
        return this;
    }
    public int getMin() {
        return _min;
    }
    public ConResponse setUpt(int upt) {
        _upt = upt;
        return this;
    }
    public int getUpt() {
        return _upt;
    }
    public ConResponse setExtentName(String name) {
        _extname = name;
        return this;
    }
    public String getExtentName() {
        return _extname;
    }
    public ConResponse setSid(Id id) {
        _sid = id;
        return this;
    }
    public Id getSid() {
        return _sid;
    }
}

