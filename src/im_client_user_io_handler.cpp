//
// im_client_user_io_handler.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#include <cstdlib>
#include <memory>
#include <boost/algorithm/string/predicate.hpp>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>
#include "im_client_user_io_handler.h"
#include "im_message.hpp"
#include "string_trim.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------

im_client_user_io_handler::im_client_user_io_handler()
{
}

//----------------------------------------------------------------------
// Public methods.
//----------------------------------------------------------------------

void im_client_user_io_handler::start( 
    im_client_user_io_handler_callback_ptr callback_ptr) 
{
  callback_ptr_ = callback_ptr;
  print_help( true );
}

void im_client_user_io_handler::process_command( 
    const std::string command )
{
  if ( !callback_ptr_)
  {
    std::cerr << "IM client user IO handler must be set first!\n";
  }
  else
  {
    //std::cout << "Received command \"" << command << "\"\n";
    if ( command.compare( HELP_CMD ) == 0)
    {
      //std::cout << "Processing 'help' command.\n";
      print_help( false );
    }
    else if ( boost::starts_with( command, CONNECT_CMD ) )
    {
      //std::cout << "Processing 'connect' command.\n";
      std::vector<std::string> command_tokens = 
          extract_command_tokens( command );

      if ( command_tokens.size() != 2 )
      {
        std::cout << "The \"" << CONNECT_CMD << "\" command accepts only one " 
          << "parameter that is the user nickname.\n";
      }
      else
      {
        std::string destinatary = trim( command_tokens.at( 1 ) );

        if ( destinatary.empty() )
        {
          std::cout << "Empty nicknames are not allowed.\n";
        }
        else if ( destinatary.length() > 
          im_message::max_destinatary_length )
        {
          std::cout << "The user nickname must not be bigger than \"" 
            << im_message::max_destinatary_length << "\".\n";
        }
        else
        {
          //std::cout << "im_client_user_io_handler::process_command -> "
            //"Connecting to server...\n";
          callback_ptr_->connect();
          //std::cout << "im_client_user_io_handler::process_command -> 
            //"Sending message...\n";
          callback_ptr_->send_message( 
            im_message::build_connect_msg( destinatary ) );
        }
      }
    }
    else if ( boost::starts_with( command, MESSAGE_CMD ) )
    {
      //std::cout << "Processing 'message' command.\n";
      std::vector<std::string> command_tokens = 
          extract_command_tokens( command );

      if ( command_tokens.size() != 2 )
      {
        std::cout << "The \"" << MESSAGE_CMD << "\" command accepts only one " 
          << "parameter that is the destinatary nickname.\n";
      }
      else
      {
        std::string destinatary = trim( command_tokens.at( 1 ) );

        if ( destinatary.empty() )
        {
          std::cout << "Empty nicknames are not allowed.\n";
        }
        else if ( destinatary.length() > 
          im_message::max_destinatary_length )
        {
          std::cout << "The user nickname must not be bigger than \"" 
            << im_message::max_destinatary_length << "\".\n";
        }
        else
        {
          destinatary_nickname = destinatary;
          is_building_msg = true;
        }
      }
    }
    else if ( command.compare( LIST_CMD ) == 0)
    {
      //std::cout << "Processing 'list' command.\n";
      callback_ptr_->send_message( im_message::build_list_request_msg() );
    }
    else if ( command.compare( QUIT_CMD ) == 0)
    {
      //std::cout << "Processing 'quit' command.\n";
      callback_ptr_->send_message( im_message::build_disconnect_msg() );
    }
    else
    {
      //std::cout << "Processing message body, or command not found.\n";
      if ( is_building_msg )
      {
        callback_ptr_->send_message( 
          im_message::build_message_msg_from_originator( 
            destinatary_nickname, command ) );

        is_building_msg = false;
      }
      else
      {
        std::cout << "# [client] said: Unrecognized command! Please, try again.\n";
      }
    }
  }
  print_next_command_dash();
}

void im_client_user_io_handler::print_user_message( 
  const std::string originator, const std::string message )
{
  //std::cout << "\n";
  std::cout << "[" << originator << "] said: " << message << "\n";
  print_next_command_dash();
}

void im_client_user_io_handler::print_server_message( 
  const std::string message )
{
  //std::cout << "\n";
  std::cout << "[server] said: " << message << "\n";
  print_next_command_dash();
}

void im_client_user_io_handler::print_nicknames_list( 
  const std::vector<std::string> nicknames_list )
{
  //std::cout << "\n";
  std::cout << "[server] said: Available users list \n";
  for ( auto nickname : nicknames_list )
  {
    std::cout << "-> " << nickname << "\n";
  }
  print_next_command_dash();
}

void im_client_user_io_handler::print_error(const std::string prefix, 
  boost::system::error_code ec)
{
  //std::cout << "\n";
  std::cerr << "[error]: " << prefix << ec.category().name() 
    << " -> " << ec.value() << "\n";
  print_next_command_dash();
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

void im_client_user_io_handler::print_help(bool with_intro)
{
  if ( with_intro )
  {
    std::cout << "################################################################################\n";
    std::cout << "#                                                                              #\n";
    std::cout << "# Welcome to message sending client!                                           #\n";
    std::cout << "#                                                                              #\n";
    std::cout << "# Below you will find the current available commands and usage explanation:    #\n";
    std::cout << "#                                                                              #\n";
  }
  else
  {
    std::cout << "################################################################################\n";
    std::cout << "#                                                                              #\n";
    std::cout << "# Current available commands and usage explanation:                            #\n";
    std::cout << "#                                                                              #\n";
  }
  std::cout << "# " << HELP_CMD 
    << ": this command will print this list of commands.                         #\n";
  std::cout << "#                                                                              #\n";
  std::cout << "# " << CONNECT_CMD << ": this command allows you to connect to " 
     << "server to start sending       #\n";
  std::cout << "#          messages                                                            #\n";
  std::cout << "#   Usage: \"" << CONNECT_CMD << " <YOUR_NICK_NAME>\", " 
     << "where \"<YOUR_NICK_NAME>\" must be        #\n";
  std::cout << "#          replaced by your nickname.                                          #\n";
  std::cout << "#          If you provide a nickname that was already registered with the      #\n";
  std::cout << "#          sending message server, the connection will be refused.             #\n";
  std::cout << "#                                                                              #\n";
  std::cout << "# " << MESSAGE_CMD << ": this command allows you to send a message" 
     << " to a specific destinatary #\n";
  std::cout << "#          at the sending messages server.                                     #\n";
  std::cout << "#   Usage: \"" << MESSAGE_CMD << " <DESTINATION_NICK_NAME>\"," 
     << " where \"<DESTINATION_NICK_NAME>\"  #\n";
  std::cout << "#          must be replaced by the nickname of the person you want to send the #\n";
  std::cout << "#          the message to.                                                     #\n";
  std::cout << "#          Everything that is typed after a \"" << MESSAGE_CMD << "\" "
    << "command is issued will   #\n";
  std::cout << "#          be the message to be sent to the previously provided destinatary.   #\n";
  std::cout << "#                                                                              #\n";
  std::cout << "# " << LIST_CMD << ": this command returns the list of nicknames " 
    << "already registered with the #\n";
  std::cout << "#       sending message server.                                                #\n";
  std::cout << "#                                                                              #\n";
  std::cout << "# " << QUIT_CMD << ": this command terminates your connection " 
    << "with the sending message       #\n";
  std::cout << "#       server.                                                                #\n";
  std::cout << "#                                                                              #\n";
  std::cout << "# It's important to note that \"" << MESSAGE_CMD << "\", \""
    << LIST_CMD << "\" and \"" << QUIT_CMD << "\" commands can only   #\n";
  std::cout << "# be executed after a successfull connection with the server.                  #\n";
  std::cout << "#                                                                              #\n";
  std::cout << "# Please, try it out!!!                                                        #\n";
  std::cout << "#                                                                              #\n";
  std::cout << "################################################################################\n";

  if ( with_intro )
  {
    print_next_command_dash();
  }
}

std::vector<std::string> im_client_user_io_handler::extract_command_tokens( 
    const std::string command )
{
  std::vector<std::string> command_tokens;

  boost::algorithm::split( command_tokens, command, 
    boost::is_any_of(" ") );

  return command_tokens;
}

void im_client_user_io_handler::print_next_command_dash()
{
  //std::cout << "\n";
  std::cout << "# ";
  std::cout.flush();
}

//----------------------------------------------------------------------
// Private fields initialization.
//----------------------------------------------------------------------

// Allowed commands.
const std::string im_client_user_io_handler::HELP_CMD = "help";
const std::string im_client_user_io_handler::CONNECT_CMD = "connect";
const std::string im_client_user_io_handler::MESSAGE_CMD = "message";
const std::string im_client_user_io_handler::LIST_CMD = "list";
const std::string im_client_user_io_handler::QUIT_CMD = "quit";

