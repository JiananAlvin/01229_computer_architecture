/*
 * Implementation for connection functions and its helper functions.
 * CSF Assignment 5 MS1
 * Jianan Xu
 * jxu147@jhu.edu
 */
#include <sstream>
#include <cctype>
#include <cassert>
#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <regex>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

/*
 * Constructor for Connection obj.
 */
Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

/*
 * Constructor for Connection obj.
 *
 * Parameters:
 *   fd - a given file descriptor
 */
Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // -TODO: call rio_readinitb to initialize the rio_t object
  this->m_fdbuf = new rio_t;
  rio_readinitb(m_fdbuf, fd);
}

/*
 * Connect to server.
 *
 * Parameters:
 *   hostname - hostname of server
 *   port     - port which server runs on
 */
void Connection::connect(const std::string &hostname, int port) {
  // -TODO: call open_clientfd to connect to the server
  const char* h = hostname.c_str();             // Convert std::string to const char *.
  const char* server_port = std::to_string(port).c_str(); // 
  int temp_fd; 
  // Call open_clientfd to connect to the server
  if ((temp_fd = open_clientfd(h, server_port)) < 0) {
    std::cerr << "Could Not Open Connection\n";
    close();
    exit(-1); // Error message should be printed in Open_client
  }
  this->m_fdbuf = new rio_t;
  Rio_readinitb(m_fdbuf, temp_fd);
  this->m_fd = temp_fd;
}

/*
 * Destructor for Connection obj.
 */
Connection::~Connection() {
  // -TODO: close the socket if it is open
  if (is_open()) {
    close();
  }
  delete m_fdbuf;
}

/*
 * Check if connection is open.
 *
 * Returns:
 *    1        - connection is open
 *    0        - connection is close
 */
bool Connection::is_open() const {
  // -TODO: return true if the connection is open
  return this->m_fd >= 0;
}

/*
 * Close connection.
 */
void Connection::close() {
  if (is_open()){
    Close(this->m_fd);
    this->m_fd = -1;
  }
}

/*
 * Send message to server.
 *
 * Parameters:
 *   msg      - message sent to server
 *
 * Returns:
 *   true     - successful
 *   false    - failed
 */
bool Connection::send(const Message &msg) {
  // -TODO: send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  // Format the message
  std::string formatted_message = msg.tag + ":" + msg.data + "\n";
  // Validate the message.
  if (!msg.hasValidTag() || (formatted_message.size() + 1 > msg.MAX_LEN)) {
    m_last_result = INVALID_MSG;
    return false;
  }

  // Send the message to server.
  size_t sent_bytes = rio_writen(m_fd, formatted_message.c_str(), formatted_message.size());
  
  // Checks whether the message was sent exactly or not.
  if (sent_bytes != formatted_message.size()) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  // Otherwise, the message was sent successfully.
  m_last_result = SUCCESS;
  return true;
}

/*
 * Receive message from server.
 *
 * Parameters:
 *   msg       - holds the response that server sends back
 *
 * Returns:
 *   true      - successful
 *   false     - failed
 */
bool Connection::receive(Message &msg) {
  // -TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately

  // Create a buffer that has the capacity of MAXLEN (255B), 
  // it will be used to store the message from server.
  char usrbuf[msg.MAX_LEN];

  // Read message from server to `usrbuf`.
  size_t received_bytes = rio_readlineb(m_fdbuf, usrbuf, sizeof(usrbuf));

  // Check whether the message was received compeletely or not.
  if (received_bytes < 0) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }
  
  // Validate Received message (stored in buffer).
  return validateReceivedMsg(msg, usrbuf);
}

/*
 * Covert char buffer to message, and then validate its structure, tag, etc.
 *
 * Parameters:
 *   msg       - holds the response (in a message form) that server sends back
 *   buf       - char buffer stored the response data
 *
 * Returns:
 *   true      - the received message is valid
 *   false     - the received message is invalid
 */
bool Connection::validateReceivedMsg(Message &msg, char* buf) {
  
  // Covert buffer to message.
  std::string tag;
  std::string data;

  if (splitToTwoPart(buf, tag, data)) {
    msg = Message(tag, data);
  } else { 
    // There is no : in the buffer, invalid message, return false.
    m_last_result = INVALID_MSG;
    return false;
  }

  // validate the `msg` (coverted from buffer).
  if (msg.tag == TAG_ERR) {
    m_last_result = EOF_OR_ERROR; 
    // But received message successfully, so should return true.
  }
  else if (!msg.hasValidTag()) {
    m_last_result = INVALID_MSG;
    // If no such a tag, return false.
    return false;
  } else {
    m_last_result = SUCCESS;
  }
  return true;
}

