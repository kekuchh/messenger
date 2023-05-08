#pragma once

#include "Server/databaseManager.h"

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include <thread>
#include <functional>
#include <unordered_map>

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class Server {
private:
    net::io_context ioc{1};
    tcp::acceptor acceptor;
    DatabaseManager dbManager;
    std::unordered_map<std::string, std::shared_ptr<tcp::socket>> clients;
public:
    explicit Server(int port);

    int Run();
private:
    void AcceptClients();

    void session(std::shared_ptr<tcp::socket> socket);
    void requestHandler(net::streambuf &buff);
    void sendResponse(tcp::socket sock, std::string msg);
};