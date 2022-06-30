#include "network.hpp"

#include <iostream>
#include <thread>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <boost/bind/placeholders.hpp>


using boost::asio::ip::udp;
#define MAX_BUFFER_SIZE 1024*32

class NetClientImpl
{
public:
    NetClientImpl(
        unsigned int listenPort, std::string ip, unsigned int port,
        FUNC_PTR_HANDLE_MSG(handleMsg), int bufferSize
    );

    ~NetClientImpl();

    void Send(const void* data, int size);

    void HandleReceiveMessage(
        const boost::system::error_code& error, std::size_t bytes_transferred
    );

    void HandleSendMessage(
        const boost::system::error_code& error, std::size_t bytes_transferred
    );
private:
    boost::asio::io_context m_ioContext;

    udp::endpoint           m_receiveEndpoint;
    udp::socket*            m_pReceiveSocket;

    udp::endpoint m_sendEndpoint;
    udp::socket*  m_pSendSocket;

    boost::array<char, MAX_BUFFER_SIZE> m_buffer;

    std::thread m_threadReceiver;

    FUNC_PTR_HANDLE_MSG(m_pMessageHandler);

    void ListenToSocket(udp::socket* pSocket, udp::endpoint& endpoint);
};

NetClient::NetClient(unsigned int listen_port, std::string ip, 
    unsigned int target_port, int bufferSize, FUNC_PTR_HANDLE_MSG(handleMsg))
{
    m_pImpl = new NetClientImpl(listen_port, ip , target_port, handleMsg, bufferSize);
}

NetClient::~NetClient()
{
    delete m_pImpl;
}

void NetClient::Send(const void* data, int size)
{
    m_pImpl->Send(data, size);
}

NetClientImpl::NetClientImpl(
    unsigned int listenPort, std::string ip, unsigned int port,
    FUNC_PTR_HANDLE_MSG(handleMsg), int bufferSize
) {
    m_pMessageHandler = handleMsg;

    // Server
    m_buffer = boost::array<char, MAX_BUFFER_SIZE>();
    m_pReceiveSocket  = new udp::socket(m_ioContext, udp::endpoint(udp::v4(), listenPort));

    ListenToSocket(m_pReceiveSocket, m_receiveEndpoint);

    // Client
    udp::resolver resolver(m_ioContext);
    m_sendEndpoint = *resolver.resolve(ip, std::to_string(port)).begin();
    m_pSendSocket    = new udp::socket(m_ioContext);
    m_pSendSocket->open(m_sendEndpoint.protocol());

    // ==========

    m_threadReceiver = std::thread([this]() {
        for (;;)
        {
            m_ioContext.run();
        }
    });
}

NetClientImpl::~NetClientImpl()
{
    m_threadReceiver.detach(); // this is wrong
    delete m_pReceiveSocket;
    delete m_pSendSocket;
}

void NetClientImpl::Send(const void* data, int size)
{
    auto handler = boost::bind(
        &NetClientImpl::HandleSendMessage,
        this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
    );
    m_pSendSocket->async_send_to(boost::asio::buffer(data, size), m_sendEndpoint, handler);
}


/*
    Handles a single message with this class's message handler.
    Endpoint will be filled with the incoming message's endpoint.
*/
void NetClientImpl::ListenToSocket(udp::socket* pSocket, udp::endpoint& endpoint)
{
    auto receiveHandler = boost::bind(
        &NetClientImpl::HandleReceiveMessage, 
        this, 
        boost::asio::placeholders::error, 
        boost::asio::placeholders::bytes_transferred
    );

    // Use instance's buffer, but this could be passed as a parameter
    // as well.
    pSocket->async_receive_from(boost::asio::buffer(m_buffer), endpoint, receiveHandler);
}

// ===================
//      Handlers

void NetClientImpl::HandleReceiveMessage(
    const boost::system::error_code& error, std::size_t bytes_transferred
) {
    if (!error) 
    {
        std::cout << "[SERVER] Message: " << m_buffer.data() << std::endl;
        m_pMessageHandler(m_buffer.data(), bytes_transferred);

        // Clear buffer for next message.
        memset(m_buffer.data(), 0, m_buffer.size());
        ListenToSocket(m_pReceiveSocket, m_receiveEndpoint);
    }
    else
    {
        std::cout << "[SERVER] " << error.what() << std::endl;
    }
}

void NetClientImpl::HandleSendMessage(
    const boost::system::error_code& error,
    std::size_t bytes_transferred
) {
    if (!error) 
    {
        std::cout << "[CLIENT] Message sent. (" << bytes_transferred << ")" << std::endl;
    }
    else
    {
        std::cout << "[CLIENT] " << error.what() << std::endl;
    }
}