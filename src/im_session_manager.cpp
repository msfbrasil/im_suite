//
// im_session_manager.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#include <cstdlib>
#include "im_session_manager.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------

im_session_manager::im_session_manager()
{

}

//----------------------------------------------------------------------
// Public methods.
//----------------------------------------------------------------------

void im_session_manager::start()
{
  im_message_handler_.start( shared_from_this() );
}

void im_session_manager::add_session( im_session_ptr im_session_ptr )
{
  boost::unique_lock<boost::mutex> scoped_lock( sessions_list_mutex );
  sessions_list.push_back( im_session_ptr );
}

void im_session_manager::remove_session( im_session_ptr im_session_ptr )
{
  boost::unique_lock<boost::mutex> scoped_lock( sessions_list_mutex );
  sessions_list.remove( im_session_ptr );
}

void im_session_manager::send_broadcast( im_message_ptr im_message_ptr )
{
  boost::unique_lock<boost::mutex> scoped_lock( sessions_list_mutex );

  for ( auto im_session_ptr : sessions_list )
  {
    //im_session_ptr->send_message( im_message_ptr );
  }
}

void im_session_manager::send_broadcast( im_message_ptr im_message_ptr, 
  std::string skip_nickname )
{
  im_session_ptr skip_nickname_session;
  {
    boost::unique_lock<boost::mutex> scoped_lock( nicknames_mutex );
    
    skip_nickname_session = nicknames_sessions_map.at( skip_nickname );
  }

  boost::unique_lock<boost::mutex> scoped_lock( sessions_list_mutex );

  for ( auto im_session_ptr : sessions_list )
  {
    if ( ( skip_nickname_session == NULL ) 
      || ( im_session_ptr != skip_nickname_session ) )
    {
      //im_session_ptr->send_message( im_message_ptr );
    }
  }
}

//----------------------------------------------------------------------

void im_session_manager::on_message_received(im_session_ptr im_session_ptr, 
  const im_message& msg)
{
  std::cout.write("Received: ", 10);
  std::cout.write(msg.value(), msg.value_length());
  std::cout << "\n";
  //im_session_ptr->send_message( msg );
  //im_session_ptr->send_message( im_message::build_connect_ack_msg( 
    //get_connection_accepted_message() ) );
  im_message_handler_.process_message( im_session_ptr, msg );
}

void im_session_manager::on_error(im_session_ptr im_session_ptr, 
  boost::system::error_code ec)
{
  std::cerr << "Communication error: " << ec.category().name()
    << " -> " << ec.value() << "\n";
  
  remove_session( im_session_ptr );
}

//----------------------------------------------------------------------

void im_session_manager::on_connect_msg( im_session_ptr im_session_ptr, 
  std::string nickname )
{
  // First we validate if the provided nickname is already registered.
  //
  if ( is_nickname_already_registered( nickname ) )
  {
    //std::cout << "Nickname already registered, sending refuse...\n";
    // TODO: this should result on client disconnection.
    im_session_ptr->send_message( im_message::build_connect_rfsd_msg( 
      get_nickname_already_connect_message( nickname ) ) );
  }
  else
  {
    //std::cout << "Registering new nickname...\n";
    register_nickname( im_session_ptr, nickname );
    //std::cout << "Sending acknowledge...\n";
    im_session_ptr->send_message( im_message::build_connect_ack_msg( 
      get_connection_accepted_message() ) );
  }
}

void im_session_manager::on_connect_ack_msg( im_session_ptr im_session_ptr, 
  std::string ack_message )
{
  // Handled by client.
}

void im_session_manager::on_connect_rfsd_msg( im_session_ptr im_session_ptr, 
  std::string error_message )
{
  // Handled by client.
}

void im_session_manager::on_message_msg( im_session_ptr im_session_ptr, 
  std::string destinatary_nickname, std::string message )
{
  boost::unique_lock<boost::mutex> scoped_lock( nicknames_mutex );

  std::cout << "Retrieving the destinatary session...\n";
  auto destinatary_session = 
    nicknames_sessions_map.at( destinatary_nickname );
  std::cout << "Destinatary session retrieved: " << destinatary_session << "\n";

  std::cout << "Registering originator nickname...\n";
  auto originator_nickname = sessions_nicknames_map.at( im_session_ptr );
  std::cout << "Originator nickname retrieved: " << originator_nickname << "\n";
  
  std::cout << "Sending message to destinatary...\n";
  destinatary_session->send_message( 
    im_message::build_message_msg_to_destinatary( 
      originator_nickname, message ) );
}

void im_session_manager::on_message_ack_msg( im_session_ptr im_session_ptr, 
  std::string ack_message )
{
  // Handled by client.
}

void im_session_manager::on_message_rfsd_msg( im_session_ptr im_session_ptr, 
  std::string error_message )
{
  // Handled by client.
}

void im_session_manager::on_list_request_msg( im_session_ptr im_session_ptr )
{
  boost::unique_lock<boost::mutex> scoped_lock( nicknames_mutex );
  std::cout << "List request received. Sending the list...\n";
  im_session_ptr->send_message( 
    im_message::build_list_response_msg( nicknames_list ) );
}

void im_session_manager::on_list_response_msg( im_session_ptr im_session_ptr, 
  std::vector<std::string> nicknames_list )
{
  // Handled by client.
}

void im_session_manager::on_disconnect_msg( im_session_ptr im_session_ptr )
{

}

void im_session_manager::on_disconnect_ack_msg( im_session_ptr im_session_ptr, 
  std::string ack_message )
{
  // Handled by client.
}

void im_session_manager::on_broadcast_msg( im_session_ptr im_session_ptr, 
  std::string nickname )
{
  // Handled by client.
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

bool im_session_manager::is_nickname_already_registered( std::string nickname )
{
  boost::unique_lock<boost::mutex> scoped_lock( nicknames_mutex );
  
  std::list<std::string>::iterator nickname_it = 
    std::find( nicknames_list.begin(), nicknames_list.end(), nickname );

  if ( nickname_it != nicknames_list.end() )
  {
    return true;
  }
  else
  {
    return false;
  }
}

void im_session_manager::register_nickname( im_session_ptr session_ptr, 
  std::string nickname )
{
  boost::unique_lock<boost::mutex> scoped_lock( nicknames_mutex );
  nicknames_list.push_back( nickname );
  nicknames_sessions_map.insert( std::pair<std::string, im_session_ptr>( nickname, session_ptr ) );
  sessions_nicknames_map.insert( std::pair<im_session_ptr, std::string>( session_ptr, nickname ) );
}

std::string im_session_manager::get_nickname_already_connect_message( 
  std::string nickname )
{
  return std::string( "A user with nickname \"" ).append( nickname ).append( 
    "\" is already connected." );
}

std::string im_session_manager::get_connection_accepted_message()
{
  return "Connection successfully established.";
}

