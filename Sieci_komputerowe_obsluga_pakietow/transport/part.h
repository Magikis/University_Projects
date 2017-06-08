// Kacper Kulczak 279079
#ifndef PART_INCLUDED
#define PART_INCLUDED 1

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <exception>
#include <list>
#include <array>

class Part
{
  size_t start;
  size_t len;
  bool recived;
  int sockfd; 
  struct sockaddr_in * server_address;
  std::array<u_int8_t, 1000> data;

public:
  Part(int s, int l, int sock_fd, struct sockaddr_in * server_address);
  void send_request();
  void recive_data(char * buffer);
  const std::array <u_int8_t, 1000 >  &  getdata() {return data;};
  size_t getstart() { return start;   }
  size_t getlen()   { return len;     }
  bool is_recived() { return recived; }
  void part_recived() { recived = true; }
};

#endif
