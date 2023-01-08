/*
 * Implementation for reciever.
 * CSF Assignment 5 MS1
 * Jianan Xu
 * jxu147@jhu.edu
 */
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

/*
 * The sender sends this command to leave the chat room they are currently in.
 *
 * Parameters:
 *   conn            - a connection of client and server
 *   leave_message   - leave message sent to server
 *   server_response - holds the response that server sends back
 *
 * Returns:
 *   0               - successful
 *   1               - EOF or error receiving or sending data
 *   2               - message format was invalid
 *   3               - received an error message from server
 */
int leave(Connection &conn, Message &leave_message, Message &server_response) {
  // Return value.
  int r; 

  // Check whether the leave message was sent successfully or not.
  r = validate_send(conn, leave_message);
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
 * Client is done, server will close the connection.
 *
 * Parameters:
 *   conn            - a connection of client and server
 *   quit_message    - quit message sent to server
 *   server_response - holds the response that server sends back
 *
 * Returns:
 *   0               - successful
 *   1               - EOF or error receiving or sending data
 *   2               - message format was invalid
 *   3               - received an error message from server
 */
int quit(Connection &conn, Message &quit_message, Message &server_response) {
  // Return value.
  int r; 

  // Check whether the leave message was sent successfully or not.
  r = validate_send(conn, quit_message);
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
 * Send a message to all users in room.
 *
 * Parameters:
 *   conn            - a connection of client and server
 *   sendall_message    - sendall message sent to server
 *   server_response - holds the response that server sends back
 *
 * Returns:
 *   0               - successful
 *   1               - EOF or error receiving or sending data
 *   2               - message format was invalid
 *   3               - received an error message from server
 */
int sendall(Connection &conn, Message &sendall_message, Message &server_response) {
  // Return value.
  int r; 

  // Check whether the leave message was sent successfully or not.
  r = validate_send(conn, sendall_message);
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
 * Run sender.
 * (1) Connect to server.
 * (2) Send slogin message.
 * (3) Loop reading commands from user, sending messages to
 *     server as appropriate.
 *
 * Parameters:
 *   argc    - argument count, the number of arguments were entered on the command line
 *   argv    - argument vector, an array of pointers to arrays of character objects
 *
 * Returns:
 *   0               - successful
 *   Otherwise       - exit with a non-zero code
 */
int main(int argc, char **argv)
{
  if (argc != 4)
  {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  Connection conn;
  struct Message slogin_message, server_response, join_message, leave_message, quit_message, sendall_message;
  int r; /* return value */

  // -TODO: connect to server---------------------------
  conn.connect(server_hostname, server_port);


  // -TODO: send slogin message-------------------------
  slogin_message = (struct Message) {TAG_SLOGIN, username};
  r = login(conn, slogin_message, server_response);
  if (r != 0) {
    return r;
  };

  // -TODO: loop reading commands from user, sending messages to
  //       server as appropriate------------------------
  std::string room_name = "";
  std::string tag;
  while (true) {
    std::cin >> tag;
    // Join room.
    if (tag == "/join") {
      std::cin >> room_name;
      join_message = (struct Message) {TAG_JOIN, room_name};
      r = join(conn, join_message, server_response);
      // If received response from the server successfully (either 0: SUCCESS/normal message
      // or 3: error message), continue, run next loop. Otherwise, return nonzero code.
      if (r == 0 || r == 3) {
        continue;
      } else {
        return r;
      }
    } else if (tag == "/leave") {
      // Leave room.
      if (room_name == "") {
        std::cerr << "You are not in a room. Pls join a room!" << std::endl;
        continue;
      }
      leave_message = (struct Message){TAG_LEAVE, ""};
      r = leave(conn, leave_message, server_response);
      // If received response from the server successfully (either 0: SUCCESS/normal message
      // or 3: error message), continue, run next loop. Otherwise, return nonzero code.
      if (r == 0 || r == 3) {
        continue;
      } else {
        return r;
      }
    }
    else if (tag == "/quit") {
      // Finish.
      quit_message = (struct Message) {TAG_QUIT, "bye"};
      r = quit(conn, quit_message, server_response);
      // If received response from the server successfully 
      //   0: SUCCESS/normal message - break, jump out of the while loop.
      //   3: error message          - continue, run next loop.
      // Otherwise, return nonzero code.
      if (r == 3) {
        continue;
      } else if (r == 1) {
        break;
      } else {
        return r;
      }
    } else { 
      // Send a message to all users in room.
      // Get message text.
      std::string content;
      std::getline(std::cin, content);
      content = tag + content;
      Message sendall_message = (struct Message) {TAG_SENDALL, content};
      r = sendall(conn, sendall_message, server_response);
      // If received response from the server successfully (either 0: SUCCESS/normal message
      // or 3: error message), continue, run next loop. Otherwise, return nonzero code.
      if (r == 0 || r == 3) {
        continue;
      } else {
        return r;
      }
    }
  }
  return 0;
} // PURE CODE 45 lines < 50 lines.


