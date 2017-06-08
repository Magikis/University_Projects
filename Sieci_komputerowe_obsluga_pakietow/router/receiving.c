//kacper kulczak 279079
#include "receiving.h"

bool configure_in_socket()
{
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
  int broadcastEnable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) )
	{
		fprintf(stderr, "setoptsock error:%s\n",strerror(errno) );
    return EXIT_FAILURE;
	}
  struct sockaddr_in server_address;
	memset(&server_address,0,sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(MYPORT);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind (sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
  return EXIT_SUCCESS;
}

bool recive_packet(struct routing_record * rec)
{
  struct sockaddr_in 	sender;
	socklen_t 			sender_len = sizeof(sender);
	u_int8_t 			buffer[IP_MAXPACKET+1];
  ssize_t datagram_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
		if (datagram_len < 0)
    {
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
			return EXIT_FAILURE;
	  }
  //nadawca
  inet_ntop(AF_INET, &(sender.sin_addr), rec->via, MAX_IP * sizeof(char));
  //info ip sieci
  inet_ntop(AF_INET, buffer, rec->net_ip, MAX_IP * sizeof(char) );
  //ifo maska sieci
  rec->netmask = buffer[4];
  //info odleglosc sieci
  unsigned int net_distance;
	memcpy(&net_distance, buffer+5, sizeof(net_distance));
  rec->distance = ntohl(net_distance);
  return EXIT_SUCCESS;
}
