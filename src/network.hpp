#pragma once

#include <boost/asio.hpp>

// buffer and buffer_size
#define FUNC_PTR_HANDLE_MSG(funcName)  void (*funcName)(void*, int)

class NetClientImpl;

/*
    Defines a generic UDP client/server that listens to a local port and sends messages
    to a remote port. Ideally client and server would have implementations in
    two different classes, but that's not the point here.
*/
class NetClient
{
public:
    NetClient(unsigned int listen_port, std::string ip, 
        unsigned int target_port, int bufferSize, FUNC_PTR_HANDLE_MSG(handleMsg));
    ~NetClient();

    void Send(const void* data, int size);
private:
    NetClientImpl* m_pImpl;
};