//
// im_client.h
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#ifndef IM_CLIENT_H
#define IM_CLIENT_H

#include <cstdlib>
#include <boost/asio.hpp>
#include "im_message.hpp"
#include "im_message_io_handler.hpp"
#include "im_client_user_io_handler.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::shared_ptr<std::thread> thread_ptr;

//----------------------------------------------------------------------

class im_client 
    : public std::enable_shared_from_this<im_client>, 
      public im_message_io_handler_callback, 
      public im_client_user_io_handler_callback
{
public:
  im_client(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator);

  //void start();
  //void write(const im_message& msg);
  void stop();

  // Inherited from im_message_io_handler_callback.
  //
  void on_message_received(const im_message& msg);
  void on_error(boost::system::error_code ec);

  // Inherited from im_client_user_io_handler_callback.
  //
  void connect();
  void send_message(const im_message& msg);

private:
  void do_connect(tcp::resolver::iterator endpoint_iterator);

private:
  boost::asio::io_service& io_service_;
  socket_ptr socket_ptr_;
  tcp::resolver::iterator endpoint_iterator_;
  im_message_io_handler im_message_io_handler_;
  thread_ptr io_service_run_thread_ptr;
};

//----------------------------------------------------------------------

typedef std::shared_ptr<im_client> im_client_ptr;

//----------------------------------------------------------------------

#endif // IM_CLIENT_H
