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

void im_session_manager::send_broadcast( const im_message& msg )
{
  boost::unique_lock<boost::mutex> scoped_lock( sessions_list_mutex );
  for ( auto im_session_ptr : sessions_list )
  {
    im_session_ptr->send_message( msg );
  }
}

void im_session_manager::on_message_received(im_session_ptr im_session_ptr, 
  const im_message& msg)
{
  std::cout.write("Received: ", 10);
  std::cout.write(msg.value(), msg.value_length());
  std::cout << "\n";
  im_session_ptr->send_message( msg );
}

void im_session_manager::on_error(im_session_ptr im_session_ptr, 
  boost::system::error_code ec)
{
  std::cerr << "Communication error: " << ec.category().name()
    << " -> " << ec.value() << "\n";
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

