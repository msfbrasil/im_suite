//
// im_server.h
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Based on "chat_server.cpp" with Copyright (c) 2013-2015 by Christopher M. 
// Kohlhoff (chris at kohlhoff dot com)
//

#ifndef IM_SERVER_H
#define IM_SERVER_H

#include <cstdlib>
#include <boost/asio.hpp>
#include "im_session_manager.h"

using boost::asio::ip::tcp;

class im_server
{
public:
  im_server(boost::asio::io_service& io_service,
      const tcp::endpoint& endpoint);

private:
  void do_accept();

  tcp::acceptor acceptor_;
  tcp::socket socket_;
  im_session_manager_ptr im_session_manager_ptr_;
};

//----------------------------------------------------------------------

#endif // IM_SERVER_H

