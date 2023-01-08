/*
 * Implementation for reciever.
 * CSF Assignment 5 MS2
 * Jianan Xu
 * jxu147@jhu.edu
 */
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"
#include "client_util.h"
#include "csapp.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// - TODO: add any additional data types that might be helpful
//       for implementing the Server member functions
struct ConnInfo {
  // int clientfd;
  Connection *conn;
  Server *server;

  ConnInfo(Connection *conn, Server *server) : conn(conn), server(server) { }
  ~ConnInfo() {
    // destroy connection when ConnInfo object is destroyed
    delete conn;
  }
};


////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

/*
 * Execute thread's task.
 *
 * Parameters:
 *   pointer to arg
 */
void *worker(void *arg) {
  // detach thread
  if (pthread_detach(pthread_self()) != 0) { 
    std::cerr << "Error detach thread!" << std::endl;
  }

  // - TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  struct ConnInfo *info = (ConnInfo*) arg;

  Message msg;

  if (!info->conn->receive(msg)) { 
    std::cerr << "Fail to receive message from client" << std::endl;
    goto DISCONNECT;
  }
  // - TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)
  if (msg.tag == TAG_RLOGIN) { 
    if (!info->conn->send((struct Message) {TAG_OK, "hello"})) { 
      std::cerr << "Fail to send message to client (receiver)" << std::endl;
      goto DISCONNECT;
    }
    User *user = new User(trim(msg.data), User::RECEIVER); 
    // Enter receiver loop.
    info->server->chat_with_receiver(user, info->conn); 
  } else if(msg.tag == TAG_SLOGIN) { 
    // Client is sneder.
    if (!info->conn->send((struct Message) {TAG_OK, "hello"})) { 
      std::cerr << "Fail to send message to client (sender)" << std::endl;
      goto DISCONNECT;
    }
    // Mark client as a sender.
    User *user = new User(trim(msg.data), User::SENDER);
    // Enter sender loop.
    info->server->chat_with_sender(user, info->conn); 
  } else { 
    // Invalid tag.
    info->conn->send((struct Message) {TAG_ERR, "Fail to login"});
    std::cerr << "Invalid tag!" << std::endl;
    goto DISCONNECT;
  }
  DISCONNECT:
  info->conn->close();
  delete info;
  return nullptr;
  }
}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////
/*
 * Server constructor.
 */
Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // - TODO: initialize mutex
  pthread_mutex_init(&m_lock, NULL);
}

/*
 * Server destructor.
 */
Server::~Server() {
  // - TODO: destroy mutex
  pthread_mutex_destroy(&m_lock);
  close(m_ssock);
}

/*
 * Use `open_listenfd` to create the server socket.
 *
 * Returns:
 *   true       - successfully
 *   false      - failed
 */
bool Server::listen() {
  // - TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  m_ssock = open_listenfd(std::to_string(m_port).c_str()); // Convert number to string.
  return m_ssock >= 0;
}

/*
 * Accept a connection on a socket; start a new
 * pthread for each connected client.
 */
void Server::handle_client_requests() {
  // - TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  while (true) {
    int clientfd = Accept(m_ssock, NULL, NULL);
    if (clientfd < 0) { 
      std::cerr << "Fail to accept client connection" << std::endl; 
    } else {
      // Updates connection information.
      struct ConnInfo *info = new ConnInfo(new Connection(clientfd), this);

      // Start new thread to handle client connection.
      pthread_t thr_id;
      if (pthread_create(&thr_id, NULL, worker, info) != 0) {
        std::cerr << "Fail to create a thread" << std::endl;
        delete info;
      }
    }
  }
}

/*
 * Check if a room exists. If not, create it.
 *
 * Parameters:
 *   room_name  - target room name
 *
 * Returns:
 *   pointer to target room.
 */
Room *Server::find_or_create_room(const std::string &room_name) {
  // - TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary  
  // If room exists, return the room pointer.
  if (m_rooms.count(room_name) > 0) { 
    return m_rooms[room_name]; 
  } else { 
    // If room doesn't exist, create and then return the room pointer.
    Room* room = new Room(room_name);
    Guard g(m_lock);
    m_rooms[room_name] = room; 
    return room; 
  }
}

/*
 * Receiver loop.
 *
 * Parameters:
 *   user       - pointer to receiver
 *   conn       - a connection of client and server
 */
void Server::chat_with_receiver(User *user, Connection* conn) {
  Message msg;
  Room *curr_room = nullptr;

  conn->receive(msg);
  if (msg.tag == TAG_JOIN) {

    // Join a room.
    curr_room = join(user,trim(msg.data));
    conn->send((struct Message){TAG_OK, "join successfully"});
  } else {
    conn->send((struct Message){TAG_ERR, "invalid tag"});
    return;
  }
  
  // Read message from the message queue.
  while (true) { 
    // Pop message out of the message queue.
    Message *msg = user->mqueue.dequeue();
    // If there is a message available in the message queue, send the message.
    if (msg != nullptr) {

      // Send message to receiver
      if (!conn->send((struct Message){msg->tag, msg->data})){
        break;
      }; 
      delete msg;
    }
  }
  conn->close();
  quit(user,curr_room);
  return;
}

/*
 * sender loop.
 *
 * Parameters:
 *   user       - pointer to sender
 *   conn       - a connection of client and server
 */
void Server::chat_with_sender(User *user, Connection* conn) {
  Message msg;
  Room *curr_room = nullptr;

  while (true) {
    conn->receive(msg);
 
    if (msg.tag == TAG_JOIN) {
      // Join a room.
      leave(user, curr_room);                 // Leave current room if applicable.
      curr_room = nullptr;                    // The user is not in the current room.
      curr_room = join(user, trim(msg.data)); // Join another room.
      if (curr_room != nullptr) {
        conn->send((struct Message){TAG_OK, "join successfully"});
      } else {
        conn->send((struct Message){TAG_ERR, "join failed sender"});
      }
    } else if (msg.tag == TAG_LEAVE) {
      // Leave a room.
      if (leave(user, curr_room)) {
        curr_room = nullptr;
        conn->send((struct Message){TAG_OK, "leave successfully"});
      } else {
        conn->send((struct Message){TAG_ERR, "leave failed"});
      }
    } else if (msg.tag == TAG_SENDALL) {
      // Send to all users in the room.
      if (sendall(user, curr_room, trim(msg.data))) {
        conn->send((struct Message){TAG_OK, "sendall successfully"});
      } else {
        conn->send((struct Message){TAG_ERR, "sendall failed"});
      }
    } else if (msg.tag == TAG_QUIT) {
      // Remove sender from user set of current room.
      if (quit(user, curr_room)) {
        curr_room = nullptr;
        conn->send((struct Message){TAG_OK, "quit successfully"});
        break;
      } else {
        conn->send((struct Message){TAG_ERR, "quit failed"});
      }
    } else {
      std::cerr<< "Invalid tag" <<std::endl;
      quit(user, curr_room); 
      conn->send((struct Message){TAG_ERR, "invalid tag"});
      break;
    }
  }
  conn->close(); 
}

/*
 * Join a room.
 *
 * Parameters:
 *   user       - pointer to user
 *   room_name  - target room name
 *
 * Returns:
 *   Pointer to the room, which the user joint.
 */
Room *Server::join(User *user, std::string room_name) {
  Room *room = find_or_create_room(room_name);
  // After finding or creating the room, add the user into it.
  room->add_member(user);
  return room;
}

/*
 * Remove the user out of current room.
 *
 * Parameters:
 *   user       - pointer to user
 *   curr_room  - pointer to current room
 *
 * Returns:
 *   true       - remove successfully
 *   false      - room doesn't exist
 */
bool Server::leave(User *user, Room *curr_room) {
  // If the room exists, broadcast message.
  if ((curr_room == nullptr) || (m_rooms.count(curr_room->get_room_name()) <= 0)) { //checks to see if a room exits in the map
    return false; 
  } else {
    // If room exists, remove the user out of current room.
    curr_room->remove_member(user);
    return true;
  }
}

/*
 * If room exists, broadcast messages to all receivers
 * in current room.
 *
 * Parameters:
 *   user       - pointer to user
 *   curr_room  - pointer to current room
 *   msg        - message text
 *
 * Returns:
 *   true       - sendall successfully
 *   false      - room doesn't exist
 */
bool Server::sendall(User *user, Room *curr_room, std::string msg) {
  if ((curr_room == nullptr) || (m_rooms.count(curr_room->get_room_name()) <= 0)) { //checks to see if a room exits in the map
    return false; 
  } else {
    // If room exists, broadcast messages to all receivers in current room.
    curr_room->broadcast_message(user->username, msg);
    return true;
  }
}

/*
 * Client is done. Client leaves the room.
 * And its pointer is deleted.
 *
 * Parameters:
 *   user       - pointer to user
 *   curr_room  - pointer to current room
 *
 * Returns:
 *   true       - quit successfully
 */
bool Server::quit(User *user, Room *curr_room) { 
  // Uer leave the room.
  leave(user, curr_room);
  // Delete the user pointer.
  delete user;
  return true;
}

