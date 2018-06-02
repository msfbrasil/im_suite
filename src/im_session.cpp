//
// im_session.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Based on "chat_server.cpp" with Copyright (c) 2013-2015 by Christopher M. 
// Kohlhoff (chris at kohlhoff dot com)
//

#include <cstdlib>
#include <iostream>
#include "im_session.h"
#include "im_message.hpp"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------

im_session::im_session(socket_ptr socket_ptr)
    : socket_ptr_(socket_ptr)
{
}

//----------------------------------------------------------------------
// Public methods.
//----------------------------------------------------------------------

void im_session::start(im_message_handler_callback_ptr callback_ptr)
{
  callback_ptr_ = callback_ptr;
  do_read_type();
}

void im_session::send_message(const im_message& msg)
{
  bool write_in_progress = !write_msgs_.empty();
  write_msgs_.push_back(msg);
  if (!write_in_progress)
  {
    do_write();
  }
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

void im_session::do_read_type()
{
  if ( !callback_ptr_)
  {
    std::cerr << "IM message handler must be set first!\n";
  }
  else {
    auto self(shared_from_this());
    boost::asio::async_read(*socket_ptr_,
        boost::asio::buffer(read_msg_.data(), im_message::type_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_type())
          {
            do_read_length();
          }
          else
          {
            callback_ptr_->on_error(self, ec);
          }
        });
  }
}

void im_session::do_read_length()
{
  if ( !callback_ptr_)
  {
    std::cerr << "IM message handler must be set first!\n";
  }
  else {
    auto self(shared_from_this());
    boost::asio::async_read(*socket_ptr_,
        boost::asio::buffer(read_msg_.header(), im_message::length_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_length())
          {
            do_read_value();
          }
          else
          {
            callback_ptr_->on_error(self, ec);
          }
        });
  }
}

void im_session::do_read_value()
{
  if ( !callback_ptr_)
  {
    std::cerr << "IM message handler must be set first!\n";
  }
  else {
    auto self(shared_from_this());
    boost::asio::async_read(*socket_ptr_,
        boost::asio::buffer(read_msg_.value(), read_msg_.value_length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            callback_ptr_->on_message_received(self, read_msg_);
            do_read_type();
          }
          else
          {
            callback_ptr_->on_error(self, ec);
          }
        });
  }
}

void im_session::do_write()
{
  if ( !callback_ptr_)
  {
    std::cerr << "IM message handler must be set first!\n";
  }
  else {
    auto self(shared_from_this());
    boost::asio::async_write(*socket_ptr_,
        boost::asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            callback_ptr_->on_error(self, ec);
          }
        });
  }
}
