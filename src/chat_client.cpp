//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "chat_message.hpp"
#include "chat_message_processor.hpp"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client 
    : public std::enable_shared_from_this<chat_client>, 
      public chat_message_processor_callback
{
public:
  chat_client(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator)
    : io_service_(io_service),
      socket_ptr_(std::make_shared<tcp::socket>(io_service)),
      endpoint_iterator_(endpoint_iterator),
      chat_message_processor_(socket_ptr_)
  {
  }

  void start_message_processing()
  {
    do_connect(endpoint_iterator_);
  }

  void write(const chat_message& msg)
  {
    io_service_.post(
        [this, msg]()
        {
          chat_message_processor_.write(msg);
        });
  }

  void on_message_received(const chat_message& msg)
  {
    std::cout.write(msg.body(), msg.body_length());
    std::cout << "\n";
  }

  void on_error(boost::system::error_code ec)
  {
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
            chat_message_processor_.start(shared_from_this());
          }
        });
  }

private:
  boost::asio::io_service& io_service_;
  socket_ptr socket_ptr_;
  tcp::resolver::iterator endpoint_iterator_;
  chat_message_processor chat_message_processor_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });

    auto chat_client_ptr = std::make_shared<chat_client>(io_service, endpoint_iterator);
    chat_client_ptr->start_message_processing();

    std::thread t([&io_service](){ io_service.run(); });

    char line[chat_message::max_body_length + 1];
    while (std::cin.getline(line, chat_message::max_body_length + 1))
    {
      chat_message msg;
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      chat_client_ptr->write(msg);
    }

    chat_client_ptr->close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

