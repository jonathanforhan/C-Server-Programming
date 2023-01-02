add_library(TcpServerLib STATIC
    TcpServerController.cpp
    TcpConnectionAcceptor.cpp
    TcpClientDbManager.cpp
    TcpClientServiceManager.cpp
    )

target_include_directories(TcpServerLib PUBLIC ".")
