//
// server_main.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//
// Based on "chat_server.cpp" with Copyright (c) 2013-2015 by Christopher M. 
// Kohlhoff (chris at kohlhoff dot com)
//

#include <cstdlib>
#include <iostream>
#include <list>
#include <boost/asio.hpp>
#include "im_server.h"

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: im_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));
    im_server im_server(io_service, endpoint);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

