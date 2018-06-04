//
// im_client.h
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#ifndef IM_CLIENT_H
#define IM_CLIENT_H

#include <cstdlib>
#include <boost/asio.hpp>
#include "im_message.hpp"
#include "im_session.h"
#include "im_client_user_io_handler.h"
#include "im_message_handler.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

class im_client 
  : public std::enable_shared_from_this<im_client>, 
    public im_session_handler_callback, 
    public im_client_user_io_handler_callback, 
    public im_message_handler_callback
{
public:
  im_client(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator, 
      im_client_user_io_handler& client_user_io_handler);

  void start();
  void stop();
  void disconnect();

  // Inherited from im_session_handler_callback.
  //
  void on_message_received(im_session_ptr im_session_ptr, 
    const im_message& msg);
  void on_error(im_session_ptr im_session_ptr, 
    boost::system::error_code ec);

  // Inherited from im_client_user_io_handler_callback.
  //
  void connect();
  bool is_connected();
  void send_message(im_message_ptr im_message_ptr);

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
  void do_connect(tcp::resolver::iterator endpoint_iterator);

private:
  boost::asio::io_service& io_service_;
  socket_ptr socket_ptr_;
  tcp::resolver::iterator endpoint_iterator_;
  im_client_user_io_handler& client_user_io_handler_;
  im_session_ptr im_session_ptr_;
  std::thread io_service_thread_;
  im_message_handler im_message_handler_;
  bool is_connected_with_server_;
};

//----------------------------------------------------------------------

typedef std::shared_ptr<im_client> im_client_ptr;

//----------------------------------------------------------------------

#endif // IM_CLIENT_H

