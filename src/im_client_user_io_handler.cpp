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
    if ( command.compare( HELP_CMD ) == 0)
    {
      print_help( false );
    }
    else if ( boost::starts_with( command, CONNECT_CMD ) )
    {
      std::vector<std::string> command_tokens = 
          extract_command_tokens( command );

      if ( command_tokens.size() != 2 )
      {
        std::cout << "The \"" << CONNECT_CMD << "\" command accepts only one " 
          << "parameter that is the user nickname.\n";
        std::cout << "\n";
      }
      else
      {
        std::string destinatary = trim( command_tokens.at( 1 ) );

        if ( destinatary.empty() )
        {
          std::cout << "Empty nicknames are not allowed.\n";
          std::cout << "\n";
        }
        else if ( destinatary.length() > 
          building_msg_.max_destinatary_length )
        {
          std::cout << "The user nickname must not be bigger than \"" 
            << building_msg_.max_destinatary_length << "\".\n";
          std::cout << "\n";
        }
        else
        {
          //callback_ptr_->connect();
          building_msg_.fill_connect_msg( destinatary.c_str() );
          callback_ptr_->delivery_message( building_msg_ );
        }
      }
    }
    else if ( boost::starts_with( command, MESSAGE_CMD ) )
    {
      std::vector<std::string> command_tokens = 
          extract_command_tokens( command );

      if ( command_tokens.size() != 2 )
      {
        std::cout << "The \"" << MESSAGE_CMD << "\" command accepts only one " 
          << "parameter that is the destinatary nickname.\n";
        std::cout << "\n";
      }
      else
      {
        std::string destinatary = trim( command_tokens.at( 1 ) );

        if ( destinatary.empty() )
        {
          std::cout << "Empty nicknames are not allowed.\n";
          std::cout << "\n";
        }
        else if ( destinatary.length() > 
          building_msg_.max_destinatary_length )
        {
          std::cout << "The user nickname must not be bigger than \"" 
            << building_msg_.max_destinatary_length << "\".\n";
          std::cout << "\n";
        }
        else
        {
          destinatary_nickname = destinatary;
          is_building_msg = true;
        }
      }
    }
    else if ( command.compare( QUIT_CMD ) == 0)
    {
      building_msg_.fill_quit_msg( command.c_str() );
      callback_ptr_->delivery_message( building_msg_ );
    }
    else
    {
      if ( is_building_msg )
      {
        building_msg_.fill_message_msg( destinatary_nickname.c_str(), 
          command.c_str() );
        callback_ptr_->delivery_message( building_msg_ );

        is_building_msg = false;
      }
      else
      {
        std::cout << "Unrecognized command! Please, try again.\n";
        std::cout << "\n";
      }
    }
  }
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

void im_client_user_io_handler::print_help(bool with_intro)
{
  if ( with_intro )
  {
    std::cout << "Welcome to message sending client!\n";
    std::cout << "\n";
    std::cout << "Below you will find the current available commands and " 
      << "usage explanation:\n";
    std::cout << "\n";
  }
  else
  {
    std::cout << "\n";
    std::cout << "Current available commands and usage explanation:\n";
    std::cout << "\n";
  }
  std::cout << HELP_CMD 
    << " : this command will print this list of commands.\n";
  std::cout << "\n";
  std::cout << CONNECT_CMD << " : this command allows you to connect to " 
     << "server to start sending messages.\n";
  std::cout << "  Usage: \"" << CONNECT_CMD << " <YOUR_NICK_NAME>\", " 
     << "where \"<YOUR_NICK_NAME>\" must be replaced by your nickname.\n";
  std::cout << "\n";
  std::cout << MESSAGE_CMD << " : this command allows you to send a message" 
     << " to a specifig destinatary at the sending messages server.\n";
  std::cout << "  Usage: \"" << MESSAGE_CMD << " <DESTINATION_NICK_NAME>\"," 
     << " where \"<DESTINATION_NICK_NAME>\" must be replaced by the nickname"
     << " of the person you want to send the message to.\n";
  std::cout << "Everything that is typed after a \"" << MESSAGE_CMD << "\" "
    << "command is issued will be the message to be sent to the provided " 
    << "destinatary.\n";
  std::cout << "\n";
  std::cout << QUIT_CMD << " : this command terminates your connection " 
    << "with the sending message server.\n";
  std::cout << "\n";
  std::cout << "It's important to note that \"" << MESSAGE_CMD << "\" and \""
    << QUIT_CMD << "\" commands can only be executed after a successfull "
    << "connection with the server.\n";
  std::cout << "\n";
  std::cout << "Please, try it out!!!\n";
  std::cout << "\n";
  std::cout << "\n";
}

std::vector<std::string> im_client_user_io_handler::extract_command_tokens( 
    const std::string command )
{
  std::vector<std::string> command_tokens;

  boost::algorithm::split( command_tokens, command, 
    boost::is_any_of(" ") );

  return command_tokens;
}

//----------------------------------------------------------------------
// Private fields initialization.
//----------------------------------------------------------------------

// Allowed commands.
const std::string im_client_user_io_handler::HELP_CMD = "help";
const std::string im_client_user_io_handler::CONNECT_CMD = "connect";
const std::string im_client_user_io_handler::MESSAGE_CMD = "message";
const std::string im_client_user_io_handler::QUIT_CMD = "quit";

