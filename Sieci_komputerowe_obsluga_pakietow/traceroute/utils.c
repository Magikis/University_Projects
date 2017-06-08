// Kacper Kulczak 279079
#include "utils.h"

u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert(length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

unsigned int count_msec(struct timeval * start, struct timeval * end  )
{
	return (end->tv_sec - start->tv_sec) * 1000 + (end->tv_usec - start->tv_usec) / 1000;
}

void balance_p_data(struct packets_info * p_data)
{
	for(int i = 0; i < p_data->number_of_packets; i++)
	{
		if( !p_data->recived[i])
		{
			int j;
			for(j = i + 1; j < PACKETS_AT_ONCE; j++)
			{
				if(p_data->recived[j])
				{
					strcpy(p_data->ip_s[i], p_data->ip_s[j]);
					p_data->recived[i] = 1;
					p_data->recived[j] = 0;
					break;
				}
			}
		}
	}
}

void compute_data(struct packets_info * p_data, struct ansewer_data * a_data)
{
	// generowanie sredniego czasu;
	if(p_data->number_of_packets == 3)
	{
		int avr_ms=0;
		for(int i=0; i< PACKETS_AT_ONCE; i++)
		{
			avr_ms += count_msec( &(p_data->start[i]),&(p_data->end[i]) );
		}
		avr_ms /= 3;
		sprintf(a_data->ms,"%d", avr_ms);
		strcat(a_data->ms,"ms");
	}
	else
	{
		strcpy(a_data->ms, "???");
	}
	//generowanie odpowiedzi skladajacej sie z ip
	balance_p_data(p_data);
	// porownywanie czy ip sa rozne
	if(p_data->number_of_packets == 0)
	{
		strcpy(a_data->ip, "*");
		strcpy(a_data->ms, "");
	}
	else
	{
		strcpy(a_data->ip, p_data->ip_s[0]);
	}
	// powtarzne adresy ip
	for(int i=1; i< p_data->number_of_packets; i++)
	{
		int diff = 0;
		for(int j = 0; j < i; j++)
		{
			if( strcmp(p_data->ip_s[i], p_data->ip_s[j]) )
			{
				diff = 1;
			}
			else
			{
				diff = 0;
				break;
			}
		}
		if(diff)
		{
			strcat(a_data->ip," " );
			strcat(a_data->ip,p_data->ip_s[i] );
		}
	}
}

int isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result == 0;
}
