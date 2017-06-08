// Kacper Kulczak 279079
#include "icmp_sending.h"


int sending( int secuence, char* ip, int sockfd, int ttl)
{
  //tworzenie naglowka
  struct icmphdr icmp_header;
  icmp_header.type = ICMP_ECHO;
  icmp_header.code = 0;
  icmp_header.un.echo.id = my_PID;
  icmp_header.un.echo.sequence = secuence;
  icmp_header.checksum = 0;
  icmp_header.checksum =compute_icmp_checksum((u_int16_t*)&icmp_header, sizeof(icmp_header));
  // tworzenie odbiorcy
  struct sockaddr_in recipient;
  memset( &recipient, 0, sizeof(recipient) );
  recipient.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &recipient.sin_addr );
  //ustawienie tll
  setsockopt( sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int) );
  //wyslanie
  ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 0, (struct sockaddr*)&recipient, sizeof(recipient) );
  if(bytes_sent < 0)
  {
    fprintf(stderr, "sendto error: %s\n", strerror(errno));
  }
  return 0;
}
