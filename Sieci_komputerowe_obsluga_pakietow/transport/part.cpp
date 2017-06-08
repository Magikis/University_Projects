// Kacper Kulczak 279079
#include "part.h"
// s dassda dasdaddasd
Part::Part(int s, int l, int sock_fd ,struct sockaddr_in * server__address)
{
  this->len = l;
  this->start = s;
  this->sockfd = sock_fd;
  this->server_address = server__address;
  this->recived = false;
}

void Part::send_request()
{
  char message[30] ;//= "GET 5 10\n";
  sprintf(message, "GET %zu %zu\n", start, len);
  size_t check_sendto = sendto(sockfd, message, strlen(message), 0,
    (struct sockaddr *) server_address, sizeof(*server_address));
  if (check_sendto != strlen(message) )
  {
    throw std::string("Sendto error: ") + strerror(errno) ;
  }
  #ifdef DEBUG
    fprintf(stderr,"start:%zu length:%zu\n",start,len);
  #endif
}

void Part::recive_data(char * buffer)
{
  for(size_t i=0; i<len; i++)
  {
    data[i] = buffer[i];
  }
  #ifdef DEBUG
    std::cout << "len:" << len << " str_len: " << data.size() << "\n";
  #endif
}
