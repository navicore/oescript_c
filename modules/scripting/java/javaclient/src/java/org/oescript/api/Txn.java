package org.oescript.api;
import java.io.IOException;

public interface Txn {

    public Txn commit() throws IOException;
    public Extent getExtent();
    public Txn rollback() throws IOException;
    public TxnStatus getStatus();
    //public long getExptime();
    public Id getId();

}

