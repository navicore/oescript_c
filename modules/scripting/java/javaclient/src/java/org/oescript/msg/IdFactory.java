package org.oescript.msg;
import org.oescript.api.Id;

public class IdFactory {

    private long _nextId = 0;
    private static Object _lock = new Object();

    public Id nextId() {
        Id id;
        synchronized (_lock) {
            id = new Id(++_nextId);
        }
        return id;
    }
}

