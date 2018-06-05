//
// im_message_publisher.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2018 by Mauro Sergio Ferreira Brasil
//

#include <cstdlib>
#include <memory>
#include "im_message_publisher.h"
#include "im_session.h"

//----------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------

im_message_publisher::im_message_publisher()
{

}

//----------------------------------------------------------------------
// Public methods.
//----------------------------------------------------------------------

void im_message_publisher::subscribe( 
  std::string topic, im_message_subscriber_ptr subscriber_ptr )
{
  boost::unique_lock<boost::mutex> scoped_lock( subscribers_list_map_mutex );
  try
  {
    auto subscribers_list = subscribers_list_map.at( topic );
    subscribers_list.push_back( subscriber_ptr );
    // Remove the map entry and reinsert updated list.
    subscribers_list_map.erase( topic );
    subscribers_list_map.insert( 
      std::pair<std::string, subscribers_ptr_list>( 
        topic, subscribers_list ) );

    std::cout << "subscribers_list size for topic \"" << topic << "\" is: " 
      << subscribers_list.size() << ".\n";
  }
  catch (std::out_of_range e)
  {
    subscribers_ptr_list subscribers_list;
    subscribers_list.push_back( subscriber_ptr );

    subscribers_list_map.insert( 
      std::pair<std::string, subscribers_ptr_list>( 
        topic, subscribers_list ) );

    std::cout << "new subscribers_list size for topic \"" << topic << "\" is: " 
      << subscribers_list.size() << ".\n";
  }

  std::cout << "subscribers_list_map size is: " 
    << subscribers_list_map.size() << ".\n";
}

void im_message_publisher::unsubscribe( 
  std::string topic, im_message_subscriber_ptr subscriber_ptr )
{
  boost::unique_lock<boost::mutex> scoped_lock( subscribers_list_map_mutex );
  try
  {
    auto subscribers_list = subscribers_list_map.at( topic );
    subscribers_list.remove( subscriber_ptr );
    if ( subscribers_list.empty() )
    {
      subscribers_list_map.erase( topic );
    }
    else
    {
      // Remove the map entry and reinsert updated list.
      subscribers_list_map.erase( topic );
      subscribers_list_map.insert( 
        std::pair<std::string, subscribers_ptr_list>( 
          topic, subscribers_list ) );
    }

    std::cout << "subscribers_list size for topic \"" << topic << "\" is: " 
      << subscribers_list.size() << ".\n";
  }
  catch (std::out_of_range e)
  {
    std::cerr << "Unsubscribe -> subscription for subscriber \"" 
      << subscriber_ptr << "\" at topic \"" << topic 
      << "\" could not be found.";
  }

  std::cout << "subscribers_list_map size is: " 
    << subscribers_list_map.size() << ".\n";
}

void im_message_publisher::publish_message( std::string topic, 
  im_message_subscriber_ptr subscriber_ptr, im_message_ptr im_message_ptr )
{
  boost::unique_lock<boost::mutex> scoped_lock( subscribers_list_map_mutex );
  try
  {
    auto subscribers_list = subscribers_list_map.at( topic );
    std::cout << "subscribers_list size for topic \"" << topic << "\" is: " 
      << subscribers_list.size() << ".\n";
    for ( auto subscriber : subscribers_list )
    {
      std::shared_ptr<im_session> session_ptr = 
        std::static_pointer_cast<im_session>(subscriber);
      if ( ( topic.compare( BROADCAST_TOPIC ) != 0 ) 
        || ( subscriber_ptr != subscriber ) )
      {
        std::cout << "Sending message to subscriber " 
          << session_ptr->get_session_owner() << ".\n";
        subscriber->process_message( im_message_ptr );
      }
      else
      {
        std::cout << "Message to subscriber " 
          << session_ptr->get_session_owner() << " won't be sent.\n";
      }
    }
  }
  catch (std::out_of_range e)
  {
    std::cerr << "Notify message -> subscription for subscriber \"" 
      << subscriber_ptr << "\" at topic \"" << topic 
      << "\" could not be found.";
  }
}

//----------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Private fields initialization.
//----------------------------------------------------------------------

// Allowed commands.
const std::string im_message_publisher::BROADCAST_TOPIC = "broadcast_topic";

