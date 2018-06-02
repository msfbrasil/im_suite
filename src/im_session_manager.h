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

//----------------------------------------------------------------------

class im_session_manager 
  : public im_message_handler_callback
{
public:
  im_session_manager();

  void add_session( im_session_ptr im_session_ptr );
  void remove_session( im_session_ptr im_session_ptr );
  void send_broadcast( const im_message& msg );

  // Inherited from im_message_io_handler_callback.
  //
  void on_message_received(im_session_ptr im_session_ptr, 
    const im_message& msg);
  void on_error(im_session_ptr im_session_ptr, 
    boost::system::error_code ec);

private:
  boost::mutex sessions_list_mutex;
  std::list<im_session_ptr> sessions_list;
};

//----------------------------------------------------------------------

typedef std::shared_ptr<im_session_manager> im_session_manager_ptr;

//----------------------------------------------------------------------

#endif // IM_SESSION_MANAGER_H
