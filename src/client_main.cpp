//
// client_main.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Based on "chat_client.cpp" with Copyright (c) 2013-2015 by Christopher M. 
// Kohlhoff (chris at kohlhoff dot com)
//

#include <cstdlib>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "im_client.h"
#include "im_message.hpp"
#include "im_client_user_io_handler.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: im_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });

    im_client_user_io_handler io_handler;

    im_client_ptr im_client_ptr = std::make_shared<im_client>( 
      io_service, endpoint_iterator, io_handler );
    im_client_ptr->start();

    io_handler.start( im_client_ptr );

    char line[im_message::max_message_length + 1];
    while (std::cin.getline(line, im_message::max_message_length + 1))
    {
      if ( !io_handler.process_command( line ) )
      {
        break;
      }
    }

    im_client_ptr->stop();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

