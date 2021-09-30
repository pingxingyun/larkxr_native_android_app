package com.pxy.cloudlarkxroculus;

public class ServerBean {
    String ip;
    String port;
    Boolean use_https;

    public String getIp() {
        return ip;
    }

    public void setIp(String ip) {
        this.ip = ip;
    }

    public String getPort() {
        return port;
    }

    public void setPort(String port) {
        this.port = port;
    }

    public Boolean getUse_https() {
        return use_https;
    }

    public void setUse_https(Boolean use_https) {
        this.use_https = use_https;
    }

    @Override
    public String toString() {
        return "ServerBean{" +
                "ip='" + ip + '\'' +
                ", port='" + port + '\'' +
                ", use_https=" + use_https +
                '}';
    }
}
