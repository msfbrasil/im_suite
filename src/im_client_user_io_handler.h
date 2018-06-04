//
// im_client_user_io_handler.h
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#ifndef IM_CLIENT_USER_IO_HANDLER_H
#define IM_CLIENT_USER_IO_HANDLER_H

#include <cstdlib>
#include <memory>
#include <vector>
#include <string>
#include "im_message.hpp"
#include <boost/asio.hpp>

//----------------------------------------------------------------------

class im_client_user_io_handler_callback
{
public:
  virtual ~im_client_user_io_handler_callback() {}
  virtual void connect() = 0;
  virtual void send_message(im_message_ptr im_message_ptr) = 0;
};

typedef std::shared_ptr<im_client_user_io_handler_callback> im_client_user_io_handler_callback_ptr;

//----------------------------------------------------------------------

class im_client_user_io_handler
{
public:
  im_client_user_io_handler();

  void start( im_client_user_io_handler_callback_ptr callback_ptr );
  void process_command( const std::string command );
  
  void print_user_message( const std::string originator, const std::string message );
  void print_server_message( const std::string message );
  void print_nicknames_list( const std::vector<std::string> nicknames_list );
  void print_error(const std::string prefix, boost::system::error_code ec);

private:
  void print_help( bool with_intro );
  std::vector<std::string> extract_command_tokens( const std::string command );
  void print_next_command_dash();

private:
  // Allowed commands.
  static const std::string HELP_CMD;
  static const std::string CONNECT_CMD;
  static const std::string MESSAGE_CMD;
  static const std::string LIST_CMD;
  static const std::string QUIT_CMD;

private:
  im_client_user_io_handler_callback_ptr callback_ptr_;
  bool is_building_msg = false;
  std::string destinatary_nickname;
  bool is_connected_with_server_;
};

#endif // IM_CLIENT_USER_IO_HANDLER_H
