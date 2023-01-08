/*
 * Implementation for reciever.
 * CSF Assignment 5 MS2
 * Jianan Xu
 * jxu147@jhu.edu
 */
#include <iostream>
#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"
#include "client_util.h"


Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // - TODO: initialize the mutex
  pthread_mutex_init(&lock, NULL);
}

Room::~Room() {
  // - TODO: destroy the mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // - TODO: add User to the room
  if (members.count(user) <= 0) {
    Guard g(lock);
    this->members.insert(user);
  }
}

void Room::remove_member(User *user) {
  // - TODO: remove User from the room
  if (members.count(user) > 0) {
    Guard g(lock);
    members.erase(members.find(user));
  }
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // - TODO: send a message to every (receiver) User in the room
  Message msg;
  msg.tag = TAG_DELIVERY;
  msg.data = trim(get_room_name()) + ":" + trim(sender_username) + ":" + trim(message_text);
  Guard g(lock);
  std::set<User *>::iterator it;
  
  // Send a message to every (receiver) user's MessageQueue in the room.
  for (it = members.begin(); it != members.end(); ++it) {
    if (!((*it)->identity==User::SENDER)) {
      (*it)->mqueue.enqueue(new Message(msg.tag, msg.data));
    }
  }
}