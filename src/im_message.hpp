//
// im_message.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Based on "chat_message.cpp" with Copyright (c) 2013-2015 by Christopher M. 
// Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef IM_MESSAGE_HPP
#define IM_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>

class im_message
{
public:
  enum { type_length = 2 };
  enum { length_length = 4 };
  enum { max_destinatary_length = 128 };
  enum { max_message_length = 512 };
  enum { default_separator_length = 2 };
  enum { max_value_length = max_destinatary_length 
    + default_separator_length + max_message_length };

  enum MessageTypes {
    CONNECT_MSG = 1,
    MESSAGE_MSG,
    QUIT_MSG
  };

  enum MessageFields {
    DST_NICK_NAME_FIELD = 1,
    MESSAGE_FIELD
  };

  const std::string DEFAULT_SEPARATOR = "||";

  //----------------------------------------------------------------------

  im_message()
    : value_length_(0)
  {
  }

  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  std::size_t length() const
  {
    return type_length + length_length + value_length_;
  }

  const char* header() const
  {
    return data_ + type_length;
  }

  char* header()
  {
    return data_ + type_length;
  }

  const char* value() const
  {
    return data_ + type_length + length_length;
  }

  char* value()
  {
    return data_ + type_length + length_length;
  }

  std::size_t value_length() const
  {
    return value_length_;
  }

  void value_length(std::size_t new_length)
  {
    value_length_ = new_length;
    if (value_length_ > max_value_length)
      value_length_ = max_value_length;
  }

  bool decode_length()
  {
    char length[length_length + 1] = "";
    std::strncat(length, data_ + type_length, length_length);
    value_length_ = std::atoi(length);
    if (value_length_ > max_value_length)
    {
      value_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_length()
  {
    char length[length_length + 1] = "";
    std::sprintf(length, "%4d", static_cast<int>(value_length_));
    std::memcpy(data_ + type_length, length, length_length);
  }

  bool decode_type()
  {
    char type[type_length + 1] = "";
    std::strncat(type, data_, type_length);
    type_ = std::atoi(type);
    if ( (type_ != CONNECT_MSG) && (type_ != MESSAGE_MSG) 
      && (type_ != QUIT_MSG) )
    {
      return false;
    }
    return true;
  }

  void encode_type()
  {
    char type[type_length + 1] = "";
    std::sprintf(type, "%2d", static_cast<int>(type_));
    std::memcpy(data_, type, type_length);
  }

  void fill_connect_msg( const char* nickname )
  {
    type_ = CONNECT_MSG;
    value_length(std::strlen(nickname));
    std::memcpy(value(), nickname, std::strlen(nickname));
    encode_type();
    encode_length();
  }

  void fill_message_msg( const char* destinatary_nickname, const char* message )
  {
    type_ = MESSAGE_MSG;
    std::string message_value = build_message_value( 
      destinatary_nickname, message );
    value_length(message_value.length());
    std::memcpy(value(), message_value.c_str(), message_value.length());
    encode_type();
    encode_length();
  }

  void fill_quit_msg( const char* command )
  {
    type_ = QUIT_MSG;
    value_length(std::strlen(command));
    std::memcpy(value(), command, std::strlen(command));
    encode_type();
    encode_length();
  }

private:
  std::string build_message_value( const char* destinatary_nickname, 
      const char* message )
  {
    return std::string( destinatary_nickname )
      .append( DEFAULT_SEPARATOR ).append( message );
  }

private:
  char data_[type_length + length_length + max_value_length];
  std::size_t value_length_;
  int type_;
};

typedef std::deque<im_message> im_message_queue;

#endif // IM_MESSAGE_HPP

