package org.oescript.api;
import java.io.IOException;

public interface Lease {

    public long getExptime();
    public Lease setDur(long newdur) throws IOException;
    public Id getId();
    public Lease cancel() throws IOException;
    public Extent getExtent();
}

