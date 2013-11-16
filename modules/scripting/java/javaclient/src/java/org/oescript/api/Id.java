package org.oescript.api;


public class Id {

    private final long _id;
    private final int _hc;

    public Id(long id) {
        _id = id;
        _hc = (new Long(_id)).hashCode() + Id.class.hashCode();
    }

    public String toString() {
        return Long.toString(_id);
    }
    public long getLong() {
        return _id;
    }

    public boolean equals(Object id) {
        return ((Id) id)._id == _id;
    }

    public int hashCode() {
        return _hc;
    }
}

