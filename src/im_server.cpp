//
// im_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Based on "chat_server.cpp" with Copyright (c) 2013-2015 by Christopher M. 
// Kohlhoff (chris at kohlhoff dot com)
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <boost/asio.hpp>
#include "im_server.h"
#include "im_session.h"
#include "logger.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------

im_server::im_server(boost::asio::io_service& io_service,
    const tcp::endpoint& endpoint)
  : acceptor_(io_service, endpoint),
    socket_(io_service)
{
  im_session_manager_ptr_ = std::make_shared<im_session_manager>();
  im_session_manager_ptr_->start();
  do_accept();
  Logger::instance();
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

void im_server::do_accept()
{
  acceptor_.async_accept(socket_,
      [this](boost::system::error_code ec)
      {
        if (!ec)
        {
          std::make_shared<im_session>(std::make_shared<tcp::socket>(
            std::move(socket_)))->start( im_session_manager_ptr_ );
        }

        do_accept();
      });
}

