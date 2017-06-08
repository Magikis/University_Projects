// Kacper Kulczak 279079
#include "utils.h"
#include "icmp_receive.h"
#include "icmp_sending.h"

#include <sys/types.h>
#include <unistd.h>

int my_PID;

int main(int args, char* argv[])
{
  if(args != 2)
  {
    printf("Wrong amount of arguments\nCorrect usage: ./traceroute ip_adress\n");
    return 1;
  }
  if( isValidIpAddress(argv[1])  )
  {
    printf("It's valid IP adress\n" );
    return 1;
  }

  my_PID = getpid();
  // Stworzenie gniazda
  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
  for(int ttl = 1; ttl <= MAX_TTL; ttl++)
  {
    struct packets_info p_data;
    memset((void *) &p_data, 0, sizeof(p_data));
    for(int i=0; i<PACKETS_AT_ONCE; i++)
    {
      sending( (ttl * PACKETS_AT_ONCE + i), argv[1], sockfd,  ttl);
      gettimeofday( &(p_data.start[i]),NULL);
    }
    struct timeval tv;
    set_time(1,&tv);
    receiving(sockfd, &tv, &p_data, ttl);
    struct ansewer_data ad;
    compute_data(&p_data, &ad);
    printf("%2d. %s %s\n",ttl,ad.ip, ad.ms );
    if(  strcmp(ad.ip, argv[1])==0  ) break;
  }
  return 0;
}
