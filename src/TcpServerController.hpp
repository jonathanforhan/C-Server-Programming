#pragma once
#ifndef TCP_SERVER_CONTROLLER_HPP
#define TCP_SERVER_CONTROLLER_HPP

#include"TcpConnectionAcceptor.hpp"
#include"TcpClientDbManager.hpp"
#include"TcpClientServiceManager.hpp"

#include<stdint.h>
#include<string>

class TcpServerController {
public:
    TcpServerController();
    ~TcpServerController();

    void Start();
    void Stop();

    uint32_t ip_addr;
    uint16_t port;
    std::string name;

private:
    TcpConnectionAcceptor* tcp_connection_acceptor;
    TcpClientDbManager* tcp_client_db_manager;
    TcpClientServiceManager* tcp_client_server_manager;

};

#endif
