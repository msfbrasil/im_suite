//
// im_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Based on "chat_client.cpp" with Copyright (c) 2013-2015 by Christopher M. 
// Kohlhoff (chris at kohlhoff dot com)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "im_client.h"
#include "im_message.hpp"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------

im_client::im_client(boost::asio::io_service& io_service,
    tcp::resolver::iterator endpoint_iterator, 
    im_client_user_io_handler& client_user_io_handler)
  : io_service_(io_service),
    socket_ptr_(std::make_shared<tcp::socket>(io_service)),
    endpoint_iterator_(endpoint_iterator),
    im_session_ptr_(std::make_shared<im_session>(socket_ptr_)),
    client_user_io_handler_(client_user_io_handler)
{
}

//----------------------------------------------------------------------
// Public methods.
//----------------------------------------------------------------------

void im_client::stop()
{
  io_service_.post([this]() { socket_ptr_->close(); });
}

void im_client::on_message_received(im_session_ptr im_session_ptr, 
  const im_message& msg)
{
  client_user_io_handler_.print_message( msg );
}

void im_client::on_error(im_session_ptr im_session_ptr, 
  boost::system::error_code ec)
{
  client_user_io_handler_.print_error( "Communication error: ", ec );
  socket_ptr_->close();
}

void im_client::connect()
{
  do_connect(endpoint_iterator_);

  io_service_thread = std::thread([&](){ io_service_.run(); });
}

void im_client::send_message(const im_message& msg)
{
  io_service_.post(
      [this, msg]()
      {
        im_session_ptr_->send_message( msg );
      });
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

void im_client::do_connect(tcp::resolver::iterator endpoint_iterator)
{
  boost::system::error_code ec;
  boost::asio::connect(*socket_ptr_, endpoint_iterator, ec);
  if (!ec)
  {
    im_session_ptr_->start( shared_from_this() );
  }
  else
  {
    client_user_io_handler_.print_error( "Error connecting to server: ", ec );
  }
}

