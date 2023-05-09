#pragma once

#include "Server/databaseManager.h"

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <thread>
#include <functional>
#include <unordered_map>
#include <vector>

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
    boost::property_tree::ptree requestToPtree(std::array<char, 1024> &buff, size_t len);
    void requestHandler(boost::property_tree::ptree &root);
    void sendResponse(std::shared_ptr<tcp::socket> socket, std::string msg);
    void successResponse(std::shared_ptr<tcp::socket> socket);
};