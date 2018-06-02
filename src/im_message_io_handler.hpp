//
// im_message_io_handler.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef IM_MESSAGE_IO_HANDLER_HPP
#define IM_MESSAGE_IO_HANDLER_HPP

#include <cstdlib>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "im_message.hpp"

using boost::asio::ip::tcp;

typedef std::shared_ptr<tcp::socket> socket_ptr;

//----------------------------------------------------------------------

class im_message_io_handler_callback
{
public:
  virtual ~im_message_io_handler_callback() {}
  virtual void on_message_received(const im_message& msg) = 0;
  virtual void on_error(boost::system::error_code ec) = 0;
};

typedef std::shared_ptr<im_message_io_handler_callback> im_message_io_handler_callback_ptr;

//----------------------------------------------------------------------

class im_message_io_handler
{
public:
  im_message_io_handler(socket_ptr socket_ptr) 
    : socket_ptr_(socket_ptr)
  {
  }

  void start(im_message_io_handler_callback_ptr callback_ptr) 
  {
    callback_ptr_ = callback_ptr;
    do_read_type();
  }

  void write(const im_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:
  void do_read_type()
  {
    if ( !callback_ptr_)
    {
      std::cerr << "IM message IO handler must be set first!\n";
    }
    else {
      auto callback(callback_ptr_);
      boost::asio::async_read(*socket_ptr_,
          boost::asio::buffer(read_msg_.data(), im_message::type_length),
          [this, callback](boost::system::error_code ec, std::size_t /*length*/)
          {
            if (!ec && read_msg_.decode_type())
            {
              do_read_length();
            }
            else
            {
              callback_ptr_->on_error(ec);
            }
          });
    }
  }

  void do_read_length()
  {
    if ( !callback_ptr_)
    {
      std::cerr << "IM message IO handler must be set first!\n";
    }
    else {
      auto callback(callback_ptr_);
      boost::asio::async_read(*socket_ptr_,
          boost::asio::buffer(read_msg_.header(), im_message::length_length),
          [this, callback](boost::system::error_code ec, std::size_t /*length*/)
          {
            if (!ec && read_msg_.decode_length())
            {
              do_read_value();
            }
            else
            {
              callback_ptr_->on_error(ec);
            }
          });
    }
  }

  void do_read_value()
  {
    if ( !callback_ptr_)
    {
      std::cerr << "IM message IO handler must be set first!\n";
    }
    else {
      auto callback(callback_ptr_);
      boost::asio::async_read(*socket_ptr_,
          boost::asio::buffer(read_msg_.value(), read_msg_.value_length()),
          [this, callback](boost::system::error_code ec, std::size_t /*length*/)
          {
            if (!ec)
            {
              callback_ptr_->on_message_received(read_msg_);
              do_read_type();
            }
            else
            {
              callback_ptr_->on_error(ec);
            }
          });
    }
  }

  void do_write()
  {
    if ( !callback_ptr_)
    {
      std::cerr << "IM message IO handler must be set first!\n";
    }
    else {
      auto callback(callback_ptr_);
      boost::asio::async_write(*socket_ptr_,
          boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
          [this, callback](boost::system::error_code ec, std::size_t /*length*/)
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
              callback_ptr_->on_error(ec);
            }
          });
    }
  }

  socket_ptr socket_ptr_;
  im_message_io_handler_callback_ptr callback_ptr_;
  im_message read_msg_;
  im_message_queue write_msgs_;
};

#endif // IM_MESSAGE_IO_HANDLER_HPP
