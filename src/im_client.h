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
#include "im_session.h"
#include "im_client_user_io_handler.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

class im_client 
    : public std::enable_shared_from_this<im_client>, 
      public im_message_handler_callback, 
      public im_client_user_io_handler_callback
{
public:
  im_client(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator, 
      im_client_user_io_handler& client_user_io_handler);

  void stop();

  // Inherited from im_message_io_handler_callback.
  //
  void on_message_received(im_session_ptr im_session_ptr, 
    const im_message& msg);
  void on_error(im_session_ptr im_session_ptr, 
    boost::system::error_code ec);

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
  im_client_user_io_handler& client_user_io_handler_;
  im_session_ptr im_session_ptr_;
  std::thread io_service_thread;
};

//----------------------------------------------------------------------

typedef std::shared_ptr<im_client> im_client_ptr;

//----------------------------------------------------------------------

#endif // IM_CLIENT_H

