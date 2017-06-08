// Kacper Kulczak 279079
#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED 1

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#define MAX_IP 20
#define PACKETS_AT_ONCE 3
#define MAX_TTL 30

struct packets_info
{
  int number_of_packets;
  int recived[PACKETS_AT_ONCE];
  char ip_s[PACKETS_AT_ONCE][MAX_IP];
  struct timeval start [PACKETS_AT_ONCE];
  struct timeval end [PACKETS_AT_ONCE];
};

struct ansewer_data
{
  int ttl;
  char ip[3*MAX_IP + 3];
  char ms[10];
};


u_int16_t compute_icmp_checksum(const void *buff, int length);
void compute_data(struct packets_info * p_data,  struct ansewer_data * a_data);
unsigned int count_msec(struct timeval * start, struct timeval * end  );
void balance_p_data(struct packets_info * p_data);
int isValidIpAddress(char *ipAddress);


#endif
