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
#include <list>
#include <boost/algorithm/string.hpp>

//----------------------------------------------------------------------

class im_message;
typedef std::shared_ptr<im_message> im_message_ptr;

//----------------------------------------------------------------------

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
    PRIOR_FIRST_MESSAGE = 0,
    CONNECT_MSG = 1,
    CONNECT_ACK_MSG,
    CONNECT_RFSD_MSG,
    MESSAGE_MSG,
    MESSAGE_ACK_MSG,
    MESSAGE_RFSD_MSG,
    LIST_REQUEST_MSG,
    LIST_RESPONSE_MSG,
    DISCONNECT_MSG,
    DISCONNECT_ACK_MSG,
    BROADCAST_MSG,
    AFTER_LAST_MESSAGE
  };

  //static const std::string DEFAULT_SEPARATOR;

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

  int type() const
  {
    return type_;
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
    if ( is_valid_message() )
    {
      return true;
    }
    return false;
  }

  void encode_type()
  {
    char type[type_length + 1] = "";
    std::sprintf(type, "%2d", static_cast<int>(type_));
    std::memcpy(data_, type, type_length);
  }

  void clear()
  {
    memset( data_, 0, type_length + length_length + max_value_length );
  }

  //----------------------------------------------------------------------

  void fill_connect_msg( const char* nickname )
  {
    type_ = CONNECT_MSG;
    value_length(std::strlen(nickname));
    std::memcpy(value(), nickname, std::strlen(nickname));
    encode_type();
    encode_length();
  }

  void fill_connect_ack_msg()
  {
    type_ = CONNECT_ACK_MSG;
    value_length(0);
    encode_type();
    encode_length();
  }

  void fill_connect_rfsd_msg( const char* error_message )
  {
    type_ = CONNECT_RFSD_MSG;
    value_length(std::strlen(error_message));
    std::memcpy(value(), error_message, std::strlen(error_message));
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

  void fill_message_ack_msg()
  {
    type_ = MESSAGE_ACK_MSG;
    value_length(0);
    encode_type();
    encode_length();
  }

  void fill_message_rfsd_msg( const char* error_message )
  {
    type_ = MESSAGE_RFSD_MSG;
    value_length(std::strlen(error_message));
    std::memcpy(value(), error_message, std::strlen(error_message));
    encode_type();
    encode_length();
  }

  void fill_list_request_msg()
  {
    type_ = LIST_REQUEST_MSG;
    value_length(0);
    encode_type();
    encode_length();
  }

  void fill_list_response_msg( const std::list<std::string> nicknames_list )
  {
    type_ = LIST_RESPONSE_MSG;
    std::string list_response_value = 
      build_list_response_value( nicknames_list );
    value_length(list_response_value.length());
    std::memcpy(value(), list_response_value.c_str(), 
      list_response_value.length());
    encode_type();
    encode_length();
  }

  void fill_disconnect_msg()
  {
    type_ = DISCONNECT_MSG;
    value_length(0);
    encode_type();
    encode_length();
  }

  void fill_disconnect_ack_msg()
  {
    type_ = DISCONNECT_ACK_MSG;
    value_length(0);
    encode_type();
    encode_length();
  }

  void fill_broadcast_msg( const char* broadcast_message )
  {
    type_ = BROADCAST_MSG;
    value_length(std::strlen(broadcast_message));
    std::memcpy(value(), broadcast_message, std::strlen(broadcast_message));
    encode_type();
    encode_length();
  }

  //----------------------------------------------------------------------

  static im_message_ptr build_connect_msg( std::string nickname )
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = CONNECT_MSG;
    new_message_ptr->value_length(nickname.length());
    std::memcpy(new_message_ptr->value(), nickname.c_str(), nickname.length());
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  static im_message_ptr build_connect_ack_msg( std::string ack_message )
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = CONNECT_ACK_MSG;
    new_message_ptr->value_length(ack_message.length());
    std::memcpy(new_message_ptr->value(), ack_message.c_str(), ack_message.length());
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  static im_message_ptr build_connect_rfsd_msg( std::string error_message )
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = CONNECT_RFSD_MSG;
    new_message_ptr->value_length(error_message.length());
    std::memcpy(new_message_ptr->value(), error_message.c_str(), 
      error_message.length());
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  static im_message_ptr build_message_msg_from_originator( 
    std::string destinatary_nickname, std::string message )
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = MESSAGE_MSG;
    std::string message_value = build_message_value( 
      destinatary_nickname, message );
    new_message_ptr->value_length(message_value.length());
    std::memcpy(new_message_ptr->value(), message_value.c_str(), 
      message_value.length());
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  static im_message_ptr build_message_msg_to_destinatary( 
    std::string originator_nickname, std::string message )
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = MESSAGE_MSG;
    std::string message_value = build_message_value( 
      originator_nickname, message );
    new_message_ptr->value_length(message_value.length());
    std::memcpy(new_message_ptr->value(), message_value.c_str(), 
      message_value.length());
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  static im_message_ptr build_message_ack_msg( std::string ack_message )
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = MESSAGE_ACK_MSG;
    new_message_ptr->value_length(ack_message.length());
    std::memcpy(new_message_ptr->value(), ack_message.c_str(), ack_message.length());
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  static im_message_ptr build_message_rfsd_msg( std::string error_message )
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = MESSAGE_RFSD_MSG;
    new_message_ptr->value_length(error_message.length());
    std::memcpy(new_message_ptr->value(), error_message.c_str(), 
      error_message.length());
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  static im_message_ptr build_list_request_msg()
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = LIST_REQUEST_MSG;
    new_message_ptr->value_length(0);
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  static im_message_ptr build_list_response_msg( 
    const std::list<std::string> nicknames_list )
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = LIST_RESPONSE_MSG;
    std::string list_response_value = 
      build_list_response_value( nicknames_list );
    new_message_ptr->value_length(list_response_value.length());
    std::memcpy(new_message_ptr->value(), list_response_value.c_str(), 
      list_response_value.length());
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  static im_message_ptr build_disconnect_msg()
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = DISCONNECT_MSG;
    new_message_ptr->value_length(0);
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  static im_message_ptr build_disconnect_ack_msg( std::string ack_message )
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = DISCONNECT_ACK_MSG;
    new_message_ptr->value_length(ack_message.length());
    std::memcpy(new_message_ptr->value(), ack_message.c_str(), 
      ack_message.length());
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  im_message_ptr build_broadcast_msg( std::string broadcast_message )
  {
    im_message_ptr new_message_ptr = std::make_shared<im_message>();
    new_message_ptr->type_ = BROADCAST_MSG;
    new_message_ptr->value_length(broadcast_message.length());
    std::memcpy(new_message_ptr->value(), broadcast_message.c_str(), 
      broadcast_message.length());
    new_message_ptr->encode_type();
    new_message_ptr->encode_length();
    return new_message_ptr;
  }

  //----------------------------------------------------------------------

  static void build_connect_msg( im_message& building_message, 
    std::string nickname )
  {
    building_message.type_ = CONNECT_MSG;
    building_message.value_length(nickname.length());
    std::memcpy(building_message.value(), nickname.c_str(), nickname.length());
    building_message.encode_type();
    building_message.encode_length();
  }

  static im_message_ptr build_connect_ack_msg( im_message& building_message, 
    std::string ack_message )
  {
    building_message.type_ = CONNECT_ACK_MSG;
    building_message.value_length(ack_message.length());
    std::memcpy(building_message.value(), ack_message.c_str(), ack_message.length());
    building_message.encode_type();
    building_message.encode_length();
  }

  static im_message_ptr build_connect_rfsd_msg( 
    im_message& building_message, std::string error_message )
  {
    building_message.type_ = CONNECT_RFSD_MSG;
    building_message.value_length(error_message.length());
    std::memcpy(building_message.value(), error_message.c_str(), 
      error_message.length());
    building_message.encode_type();
    building_message.encode_length();
  }

  static im_message_ptr build_message_msg_from_originator( 
    im_message& building_message, std::string destinatary_nickname, 
    std::string message )
  {
    building_message.type_ = MESSAGE_MSG;
    std::string message_value = build_message_value( 
      destinatary_nickname, message );
    building_message.value_length(message_value.length());
    std::memcpy(building_message.value(), message_value.c_str(), 
      message_value.length());
    building_message.encode_type();
    building_message.encode_length();
  }

  static im_message_ptr build_message_msg_to_destinatary( 
    im_message& building_message, std::string originator_nickname, 
    std::string message )
  {
    building_message.type_ = MESSAGE_MSG;
    std::string message_value = build_message_value( 
      originator_nickname, message );
    building_message.value_length(message_value.length());
    std::memcpy(building_message.value(), message_value.c_str(), 
      message_value.length());
    building_message.encode_type();
    building_message.encode_length();
  }

  static im_message_ptr build_message_ack_msg( im_message& building_message, 
    std::string ack_message )
  {
    building_message.type_ = MESSAGE_ACK_MSG;
    building_message.value_length(ack_message.length());
    std::memcpy(building_message.value(), ack_message.c_str(), ack_message.length());
    building_message.encode_type();
    building_message.encode_length();
  }

  static im_message_ptr build_message_rfsd_msg( im_message& building_message, 
    std::string error_message )
  {
    building_message.type_ = MESSAGE_RFSD_MSG;
    building_message.value_length(error_message.length());
    std::memcpy(building_message.value(), error_message.c_str(), 
      error_message.length());
    building_message.encode_type();
    building_message.encode_length();
  }

  static im_message_ptr build_list_request_msg( im_message& building_message )
  {
    building_message.type_ = LIST_REQUEST_MSG;
    building_message.value_length(0);
    building_message.encode_type();
    building_message.encode_length();
  }

  static im_message_ptr build_list_response_msg( 
    im_message& building_message, const std::list<std::string> nicknames_list )
  {
    building_message.type_ = LIST_RESPONSE_MSG;
    std::string list_response_value = 
      build_list_response_value( nicknames_list );
    building_message.value_length(list_response_value.length());
    std::memcpy(building_message.value(), list_response_value.c_str(), 
      list_response_value.length());
    building_message.encode_type();
    building_message.encode_length();
  }

  static im_message_ptr build_disconnect_msg( im_message& building_message )
  {
    building_message.type_ = DISCONNECT_MSG;
    building_message.value_length(0);
    building_message.encode_type();
    building_message.encode_length();
  }

  static im_message_ptr build_disconnect_ack_msg( 
    im_message& building_message, std::string ack_message )
  {
    building_message.type_ = DISCONNECT_ACK_MSG;
    building_message.value_length(ack_message.length());
    std::memcpy(building_message.value(), ack_message.c_str(), 
      ack_message.length());
    building_message.encode_type();
    building_message.encode_length();
  }

  im_message_ptr build_broadcast_msg( im_message& building_message, 
    std::string broadcast_message )
  {
    building_message.type_ = BROADCAST_MSG;
    building_message.value_length(broadcast_message.length());
    std::memcpy(building_message.value(), broadcast_message.c_str(), 
      broadcast_message.length());
    building_message.encode_type();
    building_message.encode_length();
  }

  //----------------------------------------------------------------------

  bool is_connect_msg() const
  {
    return type_ == CONNECT_MSG;
  }

  bool is_connect_ack_msg() const
  {
    return type_ == CONNECT_ACK_MSG;
  }

  bool is_connect_rfsd_msg() const
  {
    return type_ == CONNECT_RFSD_MSG;
  }

  bool is_message_msg() const
  {
    return type_ == MESSAGE_MSG;
  }

  bool is_message_ack_msg() const
  {
    return type_ == MESSAGE_ACK_MSG;
  }

  bool is_message_rfsd_msg() const
  {
    return type_ == MESSAGE_RFSD_MSG;
  }

  bool is_list_request_msg() const
  {
    return type_ == LIST_REQUEST_MSG;
  }

  bool is_list_response_msg() const
  {
    return type_ == LIST_RESPONSE_MSG;
  }

  bool is_disconnect_msg() const
  {
    return type_ == DISCONNECT_MSG;
  }

  bool is_disconnect_ack_msg() const
  {
    return type_ == DISCONNECT_ACK_MSG;
  }

  bool is_broadcast_msg() const
  {
    return type_ == BROADCAST_MSG;
  }

  //----------------------------------------------------------------------

  std::string get_destinatary_nickname() const
  {
    if ( is_message_msg() )
    {
      std::vector<std::string> message_elements = 
        extract_message_elements();
      
      return message_elements.at( 0 );
    }
    else
    {
      return "";
    }
  }
  
  std::string get_message_body() const
  {
    if ( is_message_msg() )
    {
      std::vector<std::string> message_elements = 
        extract_message_elements();
      
      return message_elements.at( 1 );
    }
    else
    {
      return "";
    }
  }
  
  std::vector<std::string> get_nicknames_list() const
  {
    return extract_message_elements();
  }

  //----------------------------------------------------------------------

private:
  static std::string build_message_value( std::string destinatary_nickname, 
      std::string message )
  {
    return std::string( destinatary_nickname )
      .append( "|" ).append( message );
  }

  static std::string build_list_response_value( 
      const std::list<std::string> nicknames_list )
  {
    std::string list_response;

    for ( auto nickname : nicknames_list )
    {
      std::string next_addition;

      if ( list_response.length() == 0 )
      {
        next_addition = nickname;
      }
      else
      {
        next_addition = "|" + nickname;
      }

      // TODO: Validate a better way to handle the necessity to send more 
      //       than one response given that the number of nicknames could 
      //       overcome the space available to send them in just one message.
      //
      if ( ( list_response.length() + next_addition.length() ) 
        < max_value_length )
      {
        list_response += next_addition;
      }
    }

    return list_response;
  }

  std::vector<std::string> extract_message_elements() const
  {
    std::vector<std::string> command_elements;
    const std::string value_str( value() );

    boost::algorithm::split( command_elements, value_str, 
      boost::is_any_of( "|" ) );

    return command_elements;
  }

  bool is_valid_message()
  {
    //if ( ( type_ != CONNECT_MSG) && ( type_ != CONNECT_ACK_MSG ) 
      //&& ( type_ != CONNECT_RFSD_MSG ) && ( type_ != MESSAGE_MSG ) 
      //&& ( type_ != MESSAGE_ACK_MSG ) && ( type_ != MESSAGE_RFSD_MSG ) 
      //&& ( type_ != LIST_REQUEST_MSG ) && ( type_ != LIST_RESPONSE_MSG ) 
      //&& ( type_ != DISCONNECT_MSG ) && ( type_ != DISCONNECT_ACK_MSG ) 
      //&& ( type_ != BROADCAST_MSG ) )
    if ( ( type_ > PRIOR_FIRST_MESSAGE ) && ( type_ < AFTER_LAST_MESSAGE ) )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

private:
  char data_[type_length + length_length + max_value_length];
  std::size_t value_length_;
  int type_;
};

//----------------------------------------------------------------------

//const std::string im_message::DEFAULT_SEPARATOR = "|";

//----------------------------------------------------------------------

typedef std::deque<im_message> im_message_queue;

//----------------------------------------------------------------------

#endif // IM_MESSAGE_HPP

