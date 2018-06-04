//
// im_session_manager.h
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#ifndef IM_SESSION_MANAGER_H
#define IM_SESSION_MANAGER_H

#include <cstdlib>
#include <list>
#include <boost/thread/mutex.hpp>
#include "im_session.h"
#include "im_message_handler.h"

//----------------------------------------------------------------------

class im_session_manager 
  : public std::enable_shared_from_this<im_session_manager>,
    public im_session_handler_callback, 
    public im_message_handler_callback
{
public:
  im_session_manager();

  void start();
  void add_session( im_session_ptr im_session_ptr );
  void remove_session( im_session_ptr im_session_ptr );
  void send_broadcast( im_message_ptr im_message_ptr );
  void send_broadcast( im_message_ptr im_message_ptr, 
    std::string skip_nickname );

  // Inherited from im_session_handler_callback.
  //
  void on_message_received(im_session_ptr im_session_ptr, 
    const im_message& msg);
  void on_error(im_session_ptr im_session_ptr, 
    boost::system::error_code ec);

  // Inherited from im_message_handler_callback.
  //
  void on_connect_msg( im_session_ptr im_session_ptr, 
    std::string nickname );
  void on_connect_ack_msg( im_session_ptr im_session_ptr, 
    std::string ack_message );
  void on_connect_rfsd_msg( im_session_ptr im_session_ptr, 
    std::string error_message );
  void on_message_msg( im_session_ptr im_session_ptr, 
    std::string destinatary_nickname, std::string message );
  void on_message_ack_msg( im_session_ptr im_session_ptr, 
    std::string ack_message );
  void on_message_rfsd_msg( im_session_ptr im_session_ptr, 
    std::string error_message );
  void on_list_request_msg( im_session_ptr im_session_ptr );
  void on_list_response_msg( im_session_ptr im_session_ptr, 
    std::vector<std::string> nicknames_list );
  void on_disconnect_msg( im_session_ptr im_session_ptr );
  void on_disconnect_ack_msg( im_session_ptr im_session_ptr, 
    std::string ack_message );
  void on_broadcast_msg( im_session_ptr im_session_ptr, 
    std::string broadcast_message );

private:
  bool is_nickname_already_registered( std::string nickname );
  void register_nickname( im_session_ptr session_ptr, std::string nickname );
  void unregister_session( im_session_ptr session_ptr );

  std::string get_nickname_already_connect_message( std::string nickname );
  std::string get_connection_accepted_message();
  std::string get_destinatary_not_found_message( std::string nickname );
  std::string get_message_accepted_message();
  std::string get_disconnection_accepted_message();
  std::string get_logged_in_broadcast_message( std::string nickname );
  std::string get_logged_out_broadcast_message( std::string nickname );

  int get_sessions_list_size();
  int get_nicknames_list_size();
  int get_nicknames_sessions_map_size();
  int get_sessions_nicknames_map_size();

private:
  boost::mutex sessions_list_mutex;
  std::list<im_session_ptr> sessions_list;

  boost::mutex nicknames_resources_mutex;
  std::list<std::string> nicknames_list;
  std::map<std::string, im_session_ptr> nicknames_sessions_map;
  std::map<im_session_ptr, std::string> sessions_nicknames_map;

  im_message_handler im_message_handler_;
};

//----------------------------------------------------------------------

typedef std::shared_ptr<im_session_manager> im_session_manager_ptr;

//----------------------------------------------------------------------

#endif // IM_SESSION_MANAGER_H
