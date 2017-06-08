//kacper kulczak 279079
  #ifndef RECIVE_INCLUDED
  #define RECIVE_INCLUDED 1

  #include "utils.h"

  extern int sockfd;

  bool configure_in_socket();
  bool recive_packet(struct routing_record * rec);


  #endif
