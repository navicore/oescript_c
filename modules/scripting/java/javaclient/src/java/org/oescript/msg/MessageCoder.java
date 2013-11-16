package org.oescript.msg;

public interface MessageCoder {

    public String toString(Message m) throws MessageCoderException;

    public Message fromString(String s) throws MessageCoderException;
}

