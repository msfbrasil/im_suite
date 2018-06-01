//
// client_main.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#include <cstdlib>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "im_client.h"
#include "im_message.hpp"

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

    auto im_client_ptr = std::make_shared<im_client>(io_service, endpoint_iterator);
    im_client_ptr->start_message_handler();

    std::thread t([&io_service](){ io_service.run(); });

    char line[im_message::max_body_length + 1];
    while (std::cin.getline(line, im_message::max_body_length + 1))
    {
      im_message msg;
      msg.body_length(std::strlen(line));
      std::memcpy(msg.body(), line, msg.body_length());
      msg.encode_header();
      im_client_ptr->write(msg);
    }

    im_client_ptr->close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

