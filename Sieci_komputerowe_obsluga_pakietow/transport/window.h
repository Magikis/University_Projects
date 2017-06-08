// Kacper Kulczak 279079
#ifndef WINDOW_INCLUDED
#define WINDOW_INCLUDED 1

#define WINDOW_SIZE 500
#define TIME_TO_SEND 15000
#include "part.h"



class Window
{
  int sockfd;
  struct sockaddr_in server_address;
  size_t file_len, bytes_recived, bytes_next ;
  std::fstream fout;
  u_int8_t buffer[IP_MAXPACKET+1];
  size_t recv_len, recv_start;
  int recv_offset;
  std::list <Part> win;

public:
  Window(size_t file_length);
  void init(int port, std::string file_name);
  void gen_win();
  void add_part();
  //std::queue<Part> & first_vec();
  bool recive_packet();
  bool find_propiert_part();
  bool should_increase_window() { return bytes_next < file_len;  }
  void download_file();
  void shift();
  void send_requests();
};



#endif
