// Kacper Kulczak 279079
#include "icmp_receive.h"

void print_as_bytes (unsigned char* buff, ssize_t length)
{
	for (ssize_t i = 0; i < length; i++, buff++)
		printf ("%.2x ", *buff);
}

void set_time(int x, struct timeval *tv)
{
		tv->tv_sec  = x;
		tv->tv_usec = 0;
}

int my_select(int sockfd, fd_set * descriptors, struct timeval * tv)
{
	int ready = select(sockfd+1, descriptors, NULL,NULL, tv);
	if( ready < 0 )
	{
		fprintf(stderr, "select error: %s\n",strerror(errno) );
		return EXIT_FAILURE;
	}
	if(ready == 0)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int my_recvfrom(int sockfd, u_int8_t * buffer, struct sockaddr_in * sender, socklen_t *	sender_len )
{
	ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr *)sender, sender_len);
	if (packet_len < 0) {
		fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int receiving(int sockfd, struct timeval * tv, struct packets_info  * p_data,  int ttl_rank )
{
	while (p_data->number_of_packets != PACKETS_AT_ONCE)
	{
		struct sockaddr_in 	sender;
		socklen_t	sender_len = sizeof(sender);
		u_int8_t buffer[IP_MAXPACKET];
 		u_int8_t * buffptr = buffer;
		//ustawienia select()
		fd_set descriptors;
		FD_ZERO(&descriptors);
		FD_SET(sockfd, &descriptors);
		// odbior pakietu
		if( my_select(sockfd, &descriptors, tv) ) break;
		my_recvfrom(sockfd, buffer,&sender, &sender_len);
		//zapisywanie czasy wysylania pakietu
		struct timeval temp_time;
		gettimeofday(&temp_time, NULL);

		char  sender_ip_str[MAX_IP];
		inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

		struct iphdr* ip_header = (struct iphdr*) buffer;
		u_int8_t*	icmp_packet = buffer + 4 * ip_header->ihl;
		struct icmphdr* icmp_header = (struct icmphdr*) icmp_packet;
		//Naglowek recivedICMP ( w przypadku time exceed)
		buffptr += 4 * ip_header->ihl + 8;
  	struct iphdr* rec_pck = (struct iphdr*) buffptr;
  	buffptr += (rec_pck->ihl * 4);
  	struct icmphdr* receivedICMP = (struct icmphdr*) buffptr;
		//Rozpakiwanie danych z pakietu
		int seq, id;
		if(icmp_header->type == ICMP_TIME_EXCEEDED)
		{
			seq = receivedICMP->un.echo.sequence;
			id = receivedICMP->un.echo.id;
		}
		else if(icmp_header->type == ICMP_ECHOREPLY)
		{
		 	seq = icmp_header->un.echo.sequence;
			id = icmp_header->un.echo.id;
		}
		else continue;
		//selekcja danych i zapisywanie
		if(id == my_PID && (seq / PACKETS_AT_ONCE == ttl_rank) )
		{
			strcpy(p_data->ip_s[seq % PACKETS_AT_ONCE], sender_ip_str);
			memcpy((void *)&(p_data->end[seq % PACKETS_AT_ONCE]), (void *) &temp_time, sizeof(temp_time) );
			p_data->recived[seq % PACKETS_AT_ONCE] = 1;
			p_data->number_of_packets++;
		}
	}
	return EXIT_SUCCESS;
}
