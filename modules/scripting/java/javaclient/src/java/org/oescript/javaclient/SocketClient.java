package org.oescript.javaclient;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;

public class SocketClient {

    InetAddress _serverAddr;
    Socket _socket;
    PrintWriter _out;
    BufferedReader _in;
    boolean _is_connected = false;

    public synchronized SocketClient connect(String connect_str,
                                        int port) throws IOException {

        _serverAddr = InetAddress.getByName(connect_str);
        _socket = new Socket(_serverAddr, port);
        OutputStreamWriter w =  new OutputStreamWriter(_socket .getOutputStream());
        _out = new PrintWriter(new BufferedWriter(w), true);
        _in = new BufferedReader(new InputStreamReader(_socket.getInputStream()));
        _is_connected = true;
        return this;
    }
    public synchronized SocketClient disconnect() throws IOException {
        _socket.close();
        _is_connected = false;
        return this;
    }
    public synchronized boolean isConnected() throws IOException {
        return _is_connected;
    }
    public synchronized SocketClient writeString(String msg) throws IOException {
        _out.println(msg);
        return this;
    }
    public synchronized String readString() throws IOException {
        String line = null;
        StringBuffer buf = new StringBuffer();
        while ((line = _in.readLine()) != null) {
            buf.append(line);
            if (line.equals("")) break;
        }
        if (buf.length() == 0) {
            return null;
        }
        return buf.toString();
    }
}

