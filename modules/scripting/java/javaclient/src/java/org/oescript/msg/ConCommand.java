package org.oescript.msg;


public class ConCommand extends Command {

    private int _maj, _min, _upt;
    private String _extname, _username, _pwd;

    public ConCommand setMaj(int maj) {
        _maj = maj;
        return this;
    }
    public int getMaj() {
        return _maj;
    }
    public ConCommand setMin(int min) {
        _min = min;
        return this;
    }
    public int getMin() {
        return _min;
    }
    public ConCommand setUpt(int upt) {
        _upt = upt;
        return this;
    }
    public int getUpt() {
        return _upt;
    }
    public ConCommand setExtentName(String name) {
        _extname = name;
        return this;
    }
    public String getExtentName() {
        return _extname;
    }
    public ConCommand setUsername(String name) {
        _username = name;
        return this;
    }
    public String getUsername() {
        return _username;
    }
    public ConCommand setPwd(String pwd) {
        _pwd = pwd;
        return this;
    }
    public String getPassword() {
        return _pwd;
    }
}

