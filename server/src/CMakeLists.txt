add_library(tcp_lib STATIC
            tcp_server.c
            )

target_include_directories(tcp_lib PUBLIC ".")