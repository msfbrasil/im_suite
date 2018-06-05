//
// im_message_publisher.h
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#ifndef IM_MESSAGE_PUBLISHER_H
#define IM_MESSAGE_PUBLISHER_H

#include <cstdlib>
#include <list>
#include <boost/thread/mutex.hpp>
#include "im_message_subscriber.h"

//----------------------------------------------------------------------

class im_message_publisher
{
public:

  static const std::string BROADCAST_TOPIC;

  im_message_publisher();

  void subscribe( std::string topic, 
    im_message_subscriber_ptr subscriber_ptr );
  void unsubscribe( std::string topic, 
    im_message_subscriber_ptr subscriber_ptr );
  void publish_message( std::string topic, 
    im_message_subscriber_ptr subscriber_ptr, im_message_ptr im_message_ptr );

private:
  boost::mutex subscribers_list_map_mutex;
  std::map<std::string, subscribers_ptr_list> subscribers_list_map;
};

//----------------------------------------------------------------------

#endif // IM_MESSAGE_PUBLISHER_H
