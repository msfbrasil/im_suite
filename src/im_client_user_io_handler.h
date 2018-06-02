//
// im_client_user_io_handler.h
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#ifndef IM_CLIENT_USER_IO_HANDLER_HPP
#define IM_CLIENT_USER_IO_HANDLER_HPP

#include <cstdlib>
#include <memory>
#include <vector>
#include <string>
#include "im_message.hpp"

//----------------------------------------------------------------------

class im_client_user_io_handler_callback
{
public:
  virtual ~im_client_user_io_handler_callback() {}
  virtual void connect() = 0;
  virtual void send_message(const im_message& msg) = 0;
};

typedef std::shared_ptr<im_client_user_io_handler_callback> im_client_user_io_handler_callback_ptr;

//----------------------------------------------------------------------

class im_client_user_io_handler
{
public:
  im_client_user_io_handler();

  void start( im_client_user_io_handler_callback_ptr callback_ptr );
  void process_command( const std::string command );

private:
  void print_help( bool with_intro );
  std::vector<std::string> extract_command_tokens( const std::string command );

private:
  // Allowed commands.
  static const std::string HELP_CMD;
  static const std::string CONNECT_CMD;
  static const std::string MESSAGE_CMD;
  static const std::string QUIT_CMD;

private:
  im_client_user_io_handler_callback_ptr callback_ptr_;
  im_message building_msg_;
  bool is_building_msg = false;
  std::string destinatary_nickname;
};

#endif // IM_CLIENT_USER_IO_HANDLER_HPP
