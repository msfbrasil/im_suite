//
// im_message_handler.h
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#ifndef IM_MESSAGE_HANDLER_H
#define IM_MESSAGE_HANDLER_H

#include <cstdlib>
#include <memory>
#include <vector>
#include <string>
#include <boost/asio.hpp>
#include "im_message.hpp"
#include "im_session.h"

//----------------------------------------------------------------------

class im_message_handler_callback
{
public:
  virtual ~im_message_handler_callback() {}
  virtual void on_connect_msg( im_session_ptr im_session_ptr, 
    std::string nickname ) = 0;
  virtual void on_connect_ack_msg( im_session_ptr im_session_ptr, 
    std::string ack_message ) = 0;
  virtual void on_connect_rfsd_msg( im_session_ptr im_session_ptr, 
    std::string error_message ) = 0;
  virtual void on_message_msg( im_session_ptr im_session_ptr, 
    std::string destinatary_nickname, std::string message ) = 0;
  virtual void on_message_ack_msg( im_session_ptr im_session_ptr, 
    std::string ack_message ) = 0;
  virtual void on_message_rfsd_msg( im_session_ptr im_session_ptr, 
    std::string error_message ) = 0;
  virtual void on_list_request_msg( im_session_ptr im_session_ptr ) = 0;
  virtual void on_list_response_msg( im_session_ptr im_session_ptr, 
    std::vector<std::string> nicknames_list ) = 0;
  virtual void on_disconnect_msg( im_session_ptr im_session_ptr ) = 0;
  virtual void on_disconnect_ack_msg( im_session_ptr im_session_ptr, 
    std::string ack_message ) = 0;
  virtual void on_broadcast_msg( im_session_ptr im_session_ptr, 
    std::string broadcast_message ) = 0;
};

typedef std::shared_ptr<im_message_handler_callback> im_message_handler_callback_ptr;

//----------------------------------------------------------------------

class im_message_handler
{
public:
  im_message_handler();

  void start( im_message_handler_callback_ptr callback_ptr );
  void process_message( im_session_ptr im_session_ptr, const im_message& msg );

private:
  im_message_handler_callback_ptr callback_ptr_;
};

#endif // IM_MESSAGE_HANDLER_H
