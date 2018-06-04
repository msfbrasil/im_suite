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
    : socket_ptr_(socket_ptr),
      is_connected_(true)
{
}

//----------------------------------------------------------------------
// Public methods.
//----------------------------------------------------------------------

void im_session::start(im_session_handler_callback_ptr callback_ptr)
{
  callback_ptr_ = callback_ptr;
  do_read_type();
}

void im_session::send_message(im_message_ptr im_message_ptr)
{
  //std::cout << "im_session::send_message -> Sending message...\n";
  bool write_in_progress = !write_msgs_.empty();
  //std::cout << "Pushed back message: \"" << im_message_ptr->data() << "\"\n";
  write_msgs_.push_back(im_message_ptr);
  if (!write_in_progress)
  {
    do_write();
  }
}

bool im_session::is_connected() const
{
  return is_connected_;
}

void im_session::disconnect( bool close_socket )
{
  is_connected_ = false;
  if ( close_socket )
  {
    socket_ptr_->close();
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
    //std::cout << "Call Reading type...\n";
    auto self(shared_from_this());
    boost::asio::async_read(*socket_ptr_,
        boost::asio::buffer(read_msg_.data(), im_message::type_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          //std::cout << "Read type..." << read_msg_.data() << "\n";
          if (!ec && read_msg_.decode_type())
          {
            //std::cout << "Type is: " << read_msg_.type() << "\n";
            do_read_length();
          }
          else
          {
            if ( is_connected_ )
            {
              callback_ptr_->on_error(self, ec);
            }
            disconnect( true );
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
    //std::cout << "Call Reading length...\n";
    auto self(shared_from_this());
    boost::asio::async_read(*socket_ptr_,
        boost::asio::buffer(read_msg_.header(), im_message::length_length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          //std::cout << "Read length..." << read_msg_.header() << "\n";
          if (!ec && read_msg_.decode_length())
          {
            if (read_msg_.value_length() > 0)
            {
              //std::cout << "Length: " << read_msg_.value_length() << "\n";
              do_read_value();
            }
            else
            {
              callback_ptr_->on_message_received(self, read_msg_);
              do_read_type();
            }
          }
          else
          {
            disconnect( true );
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
    //std::cout << "Call Reading value...\n";
    auto self(shared_from_this());
    boost::asio::async_read(*socket_ptr_,
        boost::asio::buffer(read_msg_.value(), read_msg_.value_length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
          //std::cout << "Read value...\n";
          if (!ec)
          {
            callback_ptr_->on_message_received(self, read_msg_);
            read_msg_.clear();
            //std::cout << "read_msg_ was cleared an now is: " << read_msg_.data() << "\n";
            do_read_type();
          }
          else
          {
            disconnect( true );
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
    //std::cout << "Preparing to send the message...\n";
    //std::cout << "Which is: \"" << write_msgs_.front()->data() << "\"\n";
    //std::cout << "Total size: \"" << write_msgs_.front()->length() << 
      //", and value_length :" << write_msgs_.front()->value_length() << "\"\n";
    boost::asio::async_write(*socket_ptr_,
        boost::asio::buffer(write_msgs_.front()->data(),
          write_msgs_.front()->length()),
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
            disconnect( true );
            callback_ptr_->on_error(self, ec);
          }
        });
  }
}
