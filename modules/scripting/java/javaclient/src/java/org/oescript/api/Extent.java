package org.oescript.api;
import java.io.IOException;
import java.util.List;
import java.util.Map;

public interface Extent {

    public Extent connect() throws IOException;
    public Extent disconnect() throws IOException;
    public Txn begin(long dur) throws IOException ;
    public Txn createTxn(Id tid, long exptime, TxnStatus status, Extent extent);

    public Lease writeList(List<String> list) throws IOException;
    public List<String> readList(List<String> templ) throws IOException;
    public List<String> takeList(List<String> templ) throws IOException;

    public Lease writeMap(Map<String, String> map) throws IOException;
    public Map<String,String> readMap(Map<String,String> templ) throws IOException;
    public Map<String,String> takeMap(Map<String,String> templ) throws IOException;

    public Extent setTimeout(long timeout);
    public long getTimeout();

    //ugly, set bytes before calling writeXXX and 
    //get bytes after calling read/takeXXX.  remember to
    //setBytes to null when writing with no attachment
    public Extent setAttachment(byte[] bytes);
    public byte[] getAttachment();

    public Extent setLeaseDur(long dur);
    public long getLeaseDur();

    public Extent setCurrentTxn(Txn txn);
    public Txn getCurrentTxn();

    public void setMsgHook(ExtentMsgHook hook);
}

