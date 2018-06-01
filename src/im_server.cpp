//
// im_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Based on "chat_server.cpp" with Copyright (c) 2013-2015 by Christopher M. 
// Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "im_message.hpp"
#include "im_message_io_handler.hpp"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<im_message> im_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const im_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_msgs_)
      participant->deliver(msg);
  }

  void leave(chat_participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const im_message& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_)
      participant->deliver(msg);
  }

private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };
  im_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class im_session
  : public chat_participant,
    public std::enable_shared_from_this<im_session>,
    public im_message_io_handler_callback
{
public:
  im_session(tcp::socket socket, chat_room& room)
    : socket_ptr_(std::make_shared<tcp::socket>(std::move(socket))),
      room_(room),
      im_message_io_handler_(socket_ptr_)
  {
  }

  void start()
  {
    room_.join(shared_from_this());
    im_message_io_handler_.start(shared_from_this());
  }

  void deliver(const im_message& msg)
  {
    im_message_io_handler_.write(msg);
  }

  void on_message_received(const im_message& msg)
  {
    std::cout.write("Received: ", 10);
    std::cout.write(msg.body(), msg.body_length());
    std::cout << "\n";
    room_.deliver(msg);
  }

  void on_error(boost::system::error_code ec)
  {
    std::cerr << "Communication error: " << ec.category().name()
      << " -> " << ec.value() << "\n";
    room_.leave(shared_from_this());
  }

private:

  socket_ptr socket_ptr_;
  im_message_io_handler im_message_io_handler_;
  chat_room& room_;
};

//----------------------------------------------------------------------

class im_server
{
public:
  im_server(boost::asio::io_service& io_service,
      const tcp::endpoint& endpoint)
    : acceptor_(io_service, endpoint),
      socket_(io_service)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec)
        {
          if (!ec)
          {
            std::make_shared<im_session>(std::move(socket_), room_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;
  chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: im_server <port> [<port> ...]\n";
      return 1;
    }

    boost::asio::io_service io_service;

    std::list<im_server> servers;
    for (int i = 1; i < argc; ++i)
    {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_service, endpoint);
    }

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

