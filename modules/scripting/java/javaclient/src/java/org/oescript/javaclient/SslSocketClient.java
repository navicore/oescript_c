package org.oescript.javaclient;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;

public class SslSocketClient extends SocketClient {

    public synchronized SocketClient connect(String connect_str, int port) throws IOException {

        _serverAddr = InetAddress.getByName(connect_str);
        _socket = new Socket(_serverAddr, port);
        OutputStreamWriter w =  new OutputStreamWriter(_socket .getOutputStream());
        _out = new PrintWriter(new BufferedWriter(w), true);
        _in = new BufferedReader(new InputStreamReader(_socket.getInputStream()));
        _is_connected = true;
        return this;
    }
    public synchronized SocketClient disconnect() throws IOException {
        super.disconnect();
        return this;
    }
    public synchronized SocketClient writeString(String msg) throws IOException {
        super.writeString(msg);
        return this;
    }
    public synchronized String readString() throws IOException {
        return super.readString();
    }
}

