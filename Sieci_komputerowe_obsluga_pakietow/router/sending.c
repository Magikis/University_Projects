//kacper kulczak 279079
#include "sending.h"

bool send_routing_table(struct routing_record * vec)
{
  for(size_t i=0; i<neighbour_size; i++ )
  {
    char brodcast_ip[MAX_IP];
    gen_brodcast_ip(neighbour_vector[i].net_ip, brodcast_ip, neighbour_vector[i].netmask);
    for(size_t j=0; j<vector_size; j++)
    {
      if( send_record( &vec[j], brodcast_ip) )
      {
        set_inf(neighbour_vector[i].net_ip, neighbour_vector[i].netmask, vec);
      }
    }
  }
  return EXIT_SUCCESS;
}



bool send_record(const struct routing_record * rec, char * reciver_ip)
{
  struct sockaddr_in server_address;
	memset(&server_address,0, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(MYPORT);
	inet_pton(AF_INET, reciver_ip, &server_address.sin_addr);
  //ip z tablicy
  u_int8_t message[9];
  inet_pton(AF_INET, rec->net_ip, message);
  // maska z tablicy
  message[4] = rec->netmask;
  //odleglosc
  unsigned int net_distance = htonl(rec->distance);
  memcpy(message+5, &net_distance, sizeof(unsigned int));
  //wyslanie
  if (sendto(sockfd, message, sizeof(message), 0, (struct sockaddr*) &server_address, sizeof(server_address)) != sizeof(message)) {
		fprintf(stderr, "sendto error: %s\n", strerror(errno));
    return EXIT_FAILURE;
	}
  return EXIT_SUCCESS;
}
