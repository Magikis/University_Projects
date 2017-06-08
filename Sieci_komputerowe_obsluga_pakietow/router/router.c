//kacper kulczak 279079
#include "utils.h"
#include "receiving.h"
#include "sending.h"


size_t vector_size = 0;
size_t neighbour_size = 0;

struct routing_record routing_vector[MAX_VECTOR_SIZE];
struct neighbour_record neighbour_vector[MAX_VECTOR_SIZE];
int sockfd;
int main()
{
  if( scanf("%lu\n", &neighbour_size ) != 1 ) return EXIT_FAILURE;
  vector_size = neighbour_size;
  // struct routing_record * routing_vector;
  // if( (neighbour_vector = (struct neighbour_record *)  malloc(sizeof(struct neighbour_record)*neighbour_size)) == NULL )
  // {
  //   printf("ERROR:NOT ENOUGH MEM FOR neighbour_vector\n" );
  //   return EXIT_FAILURE;
  // }
  // if((routing_vector = (struct routing_record *)malloc(sizeof(struct routing_record)*MAX_VECTOR_SIZE))  == NULL)
  // {
  //   printf("ERROR:NOT ENOUGH MEM FOR routing_vector\n");
  //   return EXIT_FAILURE;
  // }
  struct neighbour_record a;
  for(size_t i=0; i<neighbour_size; i++)
  {
    memset(&a, 0, sizeof(a));
    unsigned int net,dist;
    scanf("%[^/]", a.my_ip );
    scanf("/%u",&net );
    scanf(" distance %u\n",&dist );
    a.netmask = net;
    a.distance = dist;
    gen_net_ip(a.my_ip, a.net_ip, a.netmask);
    neighbour_vector[i] = a;
    copy_neighbour_rec( &a, &routing_vector[i]);
  }

  //if(TESTING)
  //{
  //  printf("TABLICA SIECI\n" );
  //  for(size_t i=0; i<neighbour_size; i++)
  //  {
  //    show_record_neighbour(neighbour_vector[i]);
  //  }
  //}


  if(configure_in_socket() )
  {
    return EXIT_FAILURE;
  }

  struct timeval my_time;
  my_time.tv_sec = ROUND_LENGTH /SHOWS_PER_ROUND;
  my_time.tv_usec = 0;
  int parts_of_round=0;
  while(1)
  {
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    int ready = select(sockfd+1, &descriptors, NULL,NULL, &my_time);
    if( ready < 0 )
    {
		fprintf(stderr, "select error: %s\n",strerror(errno) );
		return EXIT_FAILURE;
    }
    if(ready == 0)
    {
      show_vector(routing_vector);
      if(parts_of_round != 2)
      {
        parts_of_round++;
        //show_vector(routing_vector);
      }
      else
      {
        if(TESTING) printf("ROUND!!!\n" );
        parts_of_round =0;
        increase_freezd_counter(routing_vector);
        mark_unactive_nets(routing_vector);
        if( send_routing_table(routing_vector) )
        {
            if (TESTING) printf("ERROR: Sending routing table\n" );
        }
      }
      my_time.tv_sec = ROUND_LENGTH /3;
    }
    else
    {
      struct routing_record temp;
      memset(&temp, 0, sizeof(temp));
      if( recive_packet(&temp) )
      {
        if(TESTING) printf("ERROR:REFUSED PACKET\n" );
        continue;
      }
      //printf(" recived packet:   " );show_record_routing(temp);
      update_vector(routing_vector, &temp);
    }
    //printf("TAK%lu\n",my_time.tv_sec );
  }

  //send_record(&routing_vector[0], "127.0.0.1");



  return 0;
}
