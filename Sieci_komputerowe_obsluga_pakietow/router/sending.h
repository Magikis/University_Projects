//kacper kulczak 279079
#ifndef SENDING_INCLUDED
#define SENDING_INCLUDED 1

#include "utils.h"

extern int sockfd;

bool send_routing_table(struct routing_record * vec);
bool send_record(const struct routing_record * rec, char * ip);

#endif
