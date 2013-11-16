package org.oescript.util;

public class StringParse {

    //these are here because there is no obvious way to reuse the
    // URL.java parsing code without implementing a URLStreamHandler. easy
    // enough to to but means you have to deal with Security Mgr too.

    static public int parsePort(String hostStr, int defaultPort) {
        hostStr = removeProto(hostStr);
        String portStr = removeAddr(hostStr);
        int sepIdx = portStr.indexOf('/');
        if (sepIdx >= 0) {
            portStr = portStr.substring(0, sepIdx).trim();
        }
        try {
            return Integer.parseInt(portStr);
        } catch (NumberFormatException e) {
            return defaultPort;
        }
    }
    static public String parseAddr(String hostStr) {
        hostStr = removeProto(hostStr);
        int sepIdx = hostStr.indexOf(':');
        if (sepIdx < 0)  return null;
        return hostStr.substring(0, sepIdx).trim();
    }
    static public String parseExtentname(String hostStr) {
        hostStr = removeProto(hostStr);
        String portStr = removeAddr(hostStr);
        int sepIdx = portStr.indexOf('/');
        if (sepIdx < 0)  return null;
        return portStr.substring(sepIdx + 1).trim();
    }
    static public String parseProto(String hostStr) {
        int sepIdx = hostStr.indexOf('/');
        if (sepIdx < 0)  return null;
        sepIdx = hostStr.indexOf(':');
        if (sepIdx < 0)  return null;
        return hostStr.substring(0, sepIdx).trim();
    }
    static private String removeProto(String hostStr) {
        int sepIdx = hostStr.indexOf('/');
        if (sepIdx < 0)  return hostStr;
        sepIdx = hostStr.indexOf(':');
        if (sepIdx < 0)  return hostStr;
        return hostStr.substring(sepIdx + 3).trim();
    }
    static private String removeAddr(String hostStr) {
        int sepIdx = hostStr.indexOf(':');
        if (sepIdx < 0)  return hostStr;
        return hostStr.substring(sepIdx + 1).trim();
    }
}

