#ifndef CLIENT_UTIL_H
#define CLIENT_UTIL_H

#include <string>
class Connection;
struct Message;

// this header file is useful for any declarations for functions
// or classes that are used by both clients (the sender and receiver)

std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);

// you can add additional declarations here...
bool splitToTwoPart(char *s, std::string &part1, std::string &part2);
int validate_send (Connection &conn, Message &message);
int validate_receive(Connection &conn, Message &server_response);
int login(Connection &conn, Message &login_message, Message &server_response);
int join(Connection &conn, Message &join_message, Message &server_response);

#endif // CLIENT_UTIL_H
