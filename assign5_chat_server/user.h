#ifndef USER_H
#define USER_H

#include <string>
#include "message_queue.h"

struct User {

public: 
  enum IDENTITY {
      SENDER,     // sender
      RECEIVER    // receiver
    };

  std::string username;

  // queue of pending messages awaiting delivery
  MessageQueue mqueue;
  
  IDENTITY identity;

  User(const std::string &username, IDENTITY identity) : username(username), identity(identity) { }
};

#endif // USER_H
