//kacper kulczak 279079
#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED 1
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define MAX_IP 20
#define MAX_VECTOR_SIZE 20
#define INF 16
#define MYPORT 54321
#define LIM_OF_REMOVE 2
#define LIM_NO_INFO 2
#define ROUND_LENGTH 15
#define SHOWS_PER_ROUND 3
#define TESTING 0
extern size_t vector_size;

struct routing_record
{
  char net_ip[MAX_IP];
  char via[MAX_IP];
  char netmask;
  char distance;
  bool reachable;
  char rounds_unrechable;
  bool neighbour;
  bool freezd_distance;
  char rounds_freezed;
};

struct neighbour_record
{
  char net_ip[MAX_IP];
  char my_ip[MAX_IP];
  char distance;
  char netmask;

};

extern size_t vector_size ;
extern size_t neighbour_size ;
extern struct neighbour_record  neighbour_vector[MAX_VECTOR_SIZE];
extern struct routing_record routing_vector[MAX_VECTOR_SIZE];

void set_inf(char * net_ip, char mask,struct routing_record * vec );
bool find_neighbour_rec(char * ip, struct neighbour_record *  result );
bool find_routing_rec(char * ip, char netmask, struct routing_record * vec, struct routing_record ** result);
void increase_freezd_counter(struct routing_record * vec);
void vector_delete(struct routing_record * vec, size_t i);
void show_record_routing(struct routing_record r);
void show_record_neighbour(struct neighbour_record r );
void update_vector(struct routing_record * vec, struct routing_record * packet);
bool is_neighbour(struct routing_record * rec);
void copy_neighbour_rec(const struct neighbour_record * src, struct routing_record * dst );
void show_vector(struct routing_record * v);
bool mark_reached(char * ip, struct routing_record * vec);
void mark_unactive_nets(struct routing_record * vec);
unsigned int gen_mask(char m);
void gen_brodcast_ip(const char * src, char * dst, char mask );
void gen_net_ip(const char * src, char * dst, char mask);

#endif
