//
// chat_message_processor.hpp
// ~~~~~~~~~~~~~~~
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

typedef std::shared_ptr<tcp::socket> socket_ptr;

//----------------------------------------------------------------------

class chat_message_processor_callback
{
public:
  virtual ~chat_message_processor_callback() {}
  virtual void on_message_received(const chat_message& msg) = 0;
  virtual void on_error(boost::system::error_code ec) = 0;
};

typedef std::shared_ptr<chat_message_processor_callback> chat_message_processor_callback_ptr;

//----------------------------------------------------------------------

class chat_message_processor
{
public:
  chat_message_processor(socket_ptr socket_ptr) 
    : socket_ptr_(socket_ptr)
  {
  }

  void start(chat_message_processor_callback_ptr callback_ptr) 
  {
    callback_ptr_ = callback_ptr;
    do_read_header();
  }

  void write(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:
  void do_read_header()
  {
    auto callback(callback_ptr_);
    boost::asio::async_read(*socket_ptr_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, callback](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            callback_ptr_->on_error(ec);
          }
        });
  }

  void do_read_body()
  {
    auto callback(callback_ptr_);
    boost::asio::async_read(*socket_ptr_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, callback](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            callback_ptr_->on_message_received(read_msg_);
            do_read_header();
          }
          else
          {
            callback_ptr_->on_error(ec);
          }
        });
  }

  void do_write()
  {
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

  socket_ptr socket_ptr_;
  chat_message_processor_callback_ptr callback_ptr_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

