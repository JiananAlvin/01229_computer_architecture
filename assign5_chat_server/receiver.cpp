/*
 * Implementation for reciever.
 * CSF Assignment 5 MS1
 * Jianan Xu
 * jxu147@jhu.edu
 */
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

/*
 * Run receiver.
 * (1) Connect to server.
 * (2) Send rlogin and join messages.
 * (3) Loop waiting for messages from server.
 *
 * Parameters:
 *   argc    - argument count, the number of arguments were entered on the command line
 *   argv    - argument vector, an array of pointers to arrays of character objects
 *
 * Returns:
 *   0               - successful
 *   Otherwise       - exit with a non-zero code
 */
int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;
  struct Message rlogin_message, server_response, join_message, delivery_message; 
  int r; /* return value */

  // -TODO: connect to server.
  conn.connect(server_hostname, server_port);
  

  // -TODO: send rlogin and join messages (expect a response from
  //       the server for each one)
  // --------------------Send the rlogin message as its first message to the server.
  rlogin_message = (struct Message){TAG_RLOGIN, username};
  r = login(conn, rlogin_message, server_response);
  if (r != 0) {
    return r;
  };

  // --------------------Send the join message
  join_message = (struct Message){TAG_JOIN, room_name};
  r = join(conn, join_message, server_response);
  if (r != 0) {
    return r;
  };


  // -TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  // --------------------
  while (conn.receive(delivery_message)) {
    std::cout << delivery_message.tag << ": " << delivery_message.data;
    if (delivery_message.tag == TAG_DELIVERY) {
      // Info need to be printed.
      std::string data = delivery_message.data;
      std::string room;
      std::string sender_and_message;
      std::string sender;
      std::string message;
      
      // Split [room]:[sender]:[message]
      splitToTwoPart((char*) delivery_message.data.c_str(), room, sender_and_message);
      // Split [sender]:[message]
      splitToTwoPart((char*) sender_and_message.c_str(), sender, message);
      
      // Print received messages to `stdout`.
      std::cout << sender << ": " << message;
    }
    else if (delivery_message.tag == TAG_ERR) {
      std::cerr << "rececived incorrect message:" << delivery_message.data;
      return -1;
    }
  }
  return 0;
}
