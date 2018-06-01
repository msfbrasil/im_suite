//
// im_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Based on "chat_client.cpp" with Copyright (c) 2013-2015 by Christopher M. 
// Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "im_message.hpp"
#include "im_message_io_handler.hpp"

using boost::asio::ip::tcp;

typedef std::deque<im_message> im_message_queue;

class im_client 
    : public std::enable_shared_from_this<im_client>, 
      public im_message_io_handler_callback
{
public:
  im_client(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator)
    : io_service_(io_service),
      socket_ptr_(std::make_shared<tcp::socket>(io_service)),
      endpoint_iterator_(endpoint_iterator),
      im_message_io_handler_(socket_ptr_)
  {
  }

  void start_message_handler()
  {
    do_connect(endpoint_iterator_);
  }

  void write(const im_message& msg)
  {
    io_service_.post(
        [this, msg]()
        {
          im_message_io_handler_.write(msg);
        });
  }

  void on_message_received(const im_message& msg)
  {
    std::cout.write(msg.body(), msg.body_length());
    std::cout << "\n";
  }

  void on_error(boost::system::error_code ec)
  {
    std::cerr << "Communication error: " << ec.category().name() 
      << " -> " << ec.value() << "\n";
    socket_ptr_->close();
  }

  void close()
  {
    io_service_.post([this]() { socket_ptr_->close(); });
  }

private:
  void do_connect(tcp::resolver::iterator endpoint_iterator)
  {
    boost::asio::async_connect(*socket_ptr_, endpoint_iterator,
        [this](boost::system::error_code ec, tcp::resolver::iterator)
        {
          if (!ec)
          {
            im_message_io_handler_.start(shared_from_this());
          }
          else
          {
            std::cerr << "Error connecting to server: " << ec.category().name()
              << " -> " << ec.value() << "\n";
          }
        });
  }

private:
  boost::asio::io_service& io_service_;
  socket_ptr socket_ptr_;
  tcp::resolver::iterator endpoint_iterator_;
  im_message_io_handler im_message_io_handler_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: im_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });

    auto im_client_ptr = std::make_shared<im_client>(io_service, endpoint_iterator);
    im_client_ptr->start_message_handler();

    std::thread t([&io_service](){ io_service.run(); });

    char line[im_message::max_body_length + 1];
    while (std::cin.getline(line, im_message::max_body_length + 1))
    {
      im_message msg;
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      im_client_ptr->write(msg);
    }

    im_client_ptr->close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

