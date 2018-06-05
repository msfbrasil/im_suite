//
// im_message_subscriber.h
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#ifndef IM_MESSAGE_SUBSCRIBER_H
#define IM_MESSAGE_SUBSCRIBER_H

#include <cstdlib>
#include "im_message.hpp"

//----------------------------------------------------------------------

class im_message_subscriber
{
public:
  virtual ~im_message_subscriber() {}
  virtual void process_message( im_message_ptr im_message_ptr ) = 0;
};

//----------------------------------------------------------------------

typedef std::shared_ptr<im_message_subscriber> im_message_subscriber_ptr;

typedef std::list<im_message_subscriber_ptr> subscribers_ptr_list;

//----------------------------------------------------------------------

#endif // IM_MESSAGE_SUBSCRIBER_H
