//
// im_message_handler.cpp
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
#include "im_message_handler.h"
#include "im_message.hpp"
#include "string_trim.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------

im_message_handler::im_message_handler()
{
}

//----------------------------------------------------------------------
// Public methods.
//----------------------------------------------------------------------

void im_message_handler::start( 
    im_message_handler_callback_ptr callback_ptr) 
{
  callback_ptr_ = callback_ptr;
}

void im_message_handler::process_message( im_session_ptr im_session_ptr, 
  const im_message& msg )
{
  if ( !callback_ptr_)
  {
    std::cerr << "IM message handler callback must be set first!\n";
  }
  else
  {
    if ( msg.is_connect_msg() )
    {
      callback_ptr_->on_connect_msg( im_session_ptr, msg.value() );
    }
    else if ( msg.is_connect_ack_msg() )
    {
      callback_ptr_->on_connect_ack_msg( im_session_ptr, msg.value() );
    }
    else if ( msg.is_connect_rfsd_msg() )
    {
      callback_ptr_->on_connect_rfsd_msg( im_session_ptr, msg.value() );
    }
    else if ( msg.is_message_msg() )
    {
      callback_ptr_->on_message_msg( im_session_ptr, 
        msg.get_destinatary_nickname(), msg.get_message_body() );
    }
    else if ( msg.is_message_ack_msg() )
    {
      callback_ptr_->on_message_ack_msg( im_session_ptr, msg.value() );
    }
    else if ( msg.is_message_rfsd_msg() )
    {
      callback_ptr_->on_message_rfsd_msg( im_session_ptr, msg.value() );
    }
    else if ( msg.is_list_request_msg() )
    {
      callback_ptr_->on_list_request_msg( im_session_ptr );
    }
    else if ( msg.is_list_response_msg() )
    {
      callback_ptr_->on_list_response_msg( 
        im_session_ptr, msg.get_nicknames_list() );
    }
    else if ( msg.is_disconnect_msg() )
    {
      callback_ptr_->on_disconnect_msg( im_session_ptr );
    }
    else if ( msg.is_disconnect_ack_msg() )
    {
      callback_ptr_->on_disconnect_ack_msg( im_session_ptr, msg.value() );
    }
    else if ( msg.is_broadcast_msg() )
    {
      callback_ptr_->on_broadcast_msg( im_session_ptr, msg.value() );
    }
  }
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

