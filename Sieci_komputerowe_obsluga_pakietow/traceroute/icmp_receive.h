// Kacper Kulczak 279079
#ifndef RECIVE_INCLUDED
#define RECIVE_INCLUDED 1

#include "utils.h"



int receiving(int sockfd, struct timeval * tv, struct packets_info  * p_data, int ttl_rank);
void set_time(int x, struct timeval * tv);
int my_select(int sockfd, fd_set * descriptors, struct timeval * tv);
int my_recvfrom(int sockfd, u_int8_t * buffer, struct sockaddr_in * sender, socklen_t *	sender_len );
void print_as_bytes();




extern int my_PID;

#endif
