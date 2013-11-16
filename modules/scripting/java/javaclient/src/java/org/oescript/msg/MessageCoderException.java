package org.oescript.msg;
import java.io.IOException;

public class MessageCoderException extends IOException {

    private final String _msg;

    public MessageCoderException() {
        _msg = null;
    }
    public MessageCoderException(String msg) {
        _msg = msg;
    }
    public MessageCoderException(Exception e) {
        _msg = e.getMessage();
        super.setStackTrace(e.getStackTrace());
    }

    public String getMessage() {
        return _msg;
    }

}

