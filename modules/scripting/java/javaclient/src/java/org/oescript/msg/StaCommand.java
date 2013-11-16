package org.oescript.msg;
import java.util.ArrayList;
import java.util.List;

public class StaCommand extends Command {

    private long    _lease;

    public StaCommand() {
        _lease          = 60 * 1000 * 10; //10 minutes
    }

   
    public StaCommand setLeaseDur(long dur) {
        _lease = dur;
        return this;
    }
    public long getLeaseDur() {
        return _lease;
    }
}

