//
// im_session.h
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Based on "chat_server.cpp" with Copyright (c) 2013-2015 by Christopher M. 
// Kohlhoff (chris at kohlhoff dot com)
//

#ifndef IM_SESSION_H
#define IM_SESSION_H

#include <cstdlib>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include "im_message.hpp"
#include "im_message_subscriber.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::shared_ptr<tcp::socket> socket_ptr;

//----------------------------------------------------------------------

class im_session;
typedef std::shared_ptr<im_session> im_session_ptr;

//----------------------------------------------------------------------

class im_session_handler_callback
{
public:
  virtual ~im_session_handler_callback() {}
  virtual void on_message_received(im_session_ptr im_session_ptr, 
    const im_message& msg) = 0;
  virtual void on_error(im_session_ptr im_session_ptr, 
    boost::system::error_code ec) = 0;
};

typedef std::shared_ptr<im_session_handler_callback> im_session_handler_callback_ptr;

//----------------------------------------------------------------------

class im_session
  : public std::enable_shared_from_this<im_session>,
    public im_message_subscriber
{
public:
  im_session(socket_ptr socket_ptr);
  void start(im_session_handler_callback_ptr callback_ptr);
  void send_message(im_message_ptr im_message_ptr);
  const bool is_connected();
  void disconnect( bool close_socket );
  void set_session_owner( const std::string session_owner );
  std::string get_session_owner() const;

  // Inherited from im_message_subscriber.
  //
  void process_message( im_message_ptr im_message_ptr );

private:
  void do_read_type();
  void do_read_length();
  void do_read_value();
  void do_write();

private:
  socket_ptr socket_ptr_;
  im_session_handler_callback_ptr callback_ptr_;
  im_message read_msg_;
  im_message_queue write_msgs_;
  boost::mutex write_msgs_mutex_;
  bool is_connected_;
  boost::mutex is_connected_mutex_;
  std::string session_owner_;
};

//----------------------------------------------------------------------

#endif // IM_SESSION_H

