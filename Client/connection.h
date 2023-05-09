#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <unordered_map>
#include <QObject>
#include <vector>

namespace net = boost::asio;
using boost::asio::ip::tcp;

class Connection : public QObject
{
    Q_OBJECT

private:
    net::io_context ioc{1};
    tcp::socket socket;
    tcp::resolver::results_type endpoints;

public:
    Connection(std::string adress, int port);
    ~Connection();

    tcp::socket& getSocket();
    void sendToServer(std::unordered_map<std::string, std::string>& data);
    boost::property_tree::ptree receiveFromServer();
    std::string toJson(std::unordered_map<std::string, std::string>& data);

signals:
    void readyRead();
};

#endif // CONNECTION_H
