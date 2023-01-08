/*
 * Implementation helper functions for clients' connection.
 * CSF Assignment 5 MS1
 * Jianan Xu
 * jxu147@jhu.edu
 */
#include <iostream>
#include <string>
#include "message.h"
#include "client_util.h"
#include "connection.h"

// string trim functions shamelessly stolen from
// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
  return rtrim(ltrim(s));
}

/*
 * Split a string to two part. The part on the lhs of the FIRST colon
 * is the `part1`. The part on the rhs of the FIRST colon is the `part2`.
 * The delimiter is always ':'.
 * Ex.   s   is   [room]:[sender]:[message]
 *     part1 is   [room]
 *     part2 is   [sender]:[message]
 *
 * Parameters:
 *   s         - sourse string
 *   part1     - the left sub string
 *   part2     - the right sub string
 *
 * Returns:
 *   true       - successful
 *   false      - failed
 */
bool splitToTwoPart(char *s, std::string &part1, std::string &part2) {

  // Parse (split) a string using string delimiter.
  std::string str = s;
  std::string delimiter = ":";
  size_t pos = 0;
  if (((pos = str.find(delimiter)) != std::string::npos)) {
    part1 = str.substr(0, pos);
    str.erase(0, pos + delimiter.length());
    // The leftover part of `str` is the part2.
    part2 = str;
    return true;
  } else {
    // There is no : in the buffer, return NULL.
    return false;
  }
}

/*
 * Validate send, and give proper error messages if needed.
 *
 * Parameters:
 *   conn            - a connection of client and server
 *   message         - message sent to server
 *
 * Returns:
 *   0               - successful
 *   1               - EOF or error receiving or sending data
 *   2               - message format was invalid
 */
int validate_send (Connection &conn, Message &message) {
  if (!conn.send(message)) {
    if (conn.get_last_result() == conn.INVALID_MSG) {
      std::cerr << "Failed login: sending message format was invalid." << std::endl;
      return 2;
    } else {
      std::cerr << "Failed login: EOF or error sending data." << std::endl;
      return 1;
    }
  }
  return 0;
}

/*
 * Validate receive, and give proper error messages if needed.
 *
 * Parameters:
 *   conn            - a connection of client and server
 *   server_response - holds the response that server sends back
 *
 * Returns:
 *   0               - successful
 *   1               - EOF or error receiving or sending data
 *   2               - message format was invalid
 */
int validate_receive(Connection &conn, Message &server_response) {
  if (!conn.receive(server_response)) {
    if (conn.get_last_result() == conn.INVALID_MSG) {
      std::cerr << "Failed login: receiving message format was invalid." << std::endl;
      return 2;
    } else {
      std::cerr << "Failed login: EOF or error receiving data." << std::endl;
      return 1;
    }
  }
  return 0;
}

/*
 * log in as sender/reciever. Depends on the input login message.
 *
 * Parameters:
 *   conn            - a connection of client and server
 *   login_message   - r/s login message sent to server
 *   server_response - holds the response that server sends back
 *
 * Returns:
 *   0               - successful
 *   1               - EOF or error receiving or sending data
 *   2               - message format was invalid
 *   3               - received an error message from server
 */
int login(Connection &conn, Message &login_message, Message &server_response) {
  // Return value.
  int r;

  // Check whether the login message was sent successfully or not.
  r = validate_send(conn, login_message);
  if (r != 0) {
    return r;
  }

  // Check whether the response was received from server successfully or not.
  r = validate_receive(conn, server_response);
  if (r != 0) {
    return r;
  }

  // Check whether the received message is an error message or not.
  // The error text printed must be exactly the payload returned from the server in the err message.
  if (server_response.tag == TAG_ERR) {
    std::cerr << server_response.data;
    return 3;
  }
  return 0;
}

/*
 * Client wants to join specified room (which will be created as necessary). 
 * Client leaves the current room if applicable.
 *
 * Parameters:
 *   conn            - a connection of client and server
 *   join_message   - join message sent to server
 *   server_response - holds the response that server sends back
 *
 * Returns:
 *   0               - successful
 *   1               - EOF or error receiving or sending data
 *   2               - message format was invalid
 *   3               - received an error message from server
 */
int join(Connection &conn, Message &join_message, Message &server_response) {
  // Return value.
  int r;

  // Check whether the join message was sent successfully or not.
  r = validate_send(conn, join_message);
  if (r != 0) {
    return r;
  }

  // Check whether the response was received from server successfully or not.
  r = validate_receive(conn, server_response);
  if (r != 0) {
    return r;
  }
  
  // Check whether the received message is an error message or not.
  // The error text printed must be exactly the payload returned from the server in the err message.
  if (server_response.tag == TAG_ERR) {
    std::cerr << server_response.data;
    return 3;
  }
  return 0;
}
