//kacper kulczak 279079
#include "utils.h"


void set_inf(char * net_ip, char mask,struct routing_record * vec )
{
  for(size_t i=0; i< vector_size; i++)
  {
    char from_net[MAX_IP];
    gen_net_ip(vec[i].via, from_net, mask);
    if( strcmp(vec[i].net_ip, net_ip) == 0 || strcmp(from_net,net_ip ) == 0 )
    {
      vec[i].freezd_distance=true;
      vec[i].distance = INF;
    }
  }
}

bool find_neighbour_rec(char * ip, struct neighbour_record *  result )
{
  char net_ip[MAX_IP];
  for(size_t i=0; i<neighbour_size; i++)
  {
    gen_net_ip(ip, net_ip, neighbour_vector[i].netmask);
    if( strcmp(neighbour_vector[i].net_ip  ,net_ip) == 0 && strcmp(ip,neighbour_vector[i].my_ip )!=0 )
    {
      *result = neighbour_vector[i];
      return EXIT_SUCCESS;
    }
  }
  return EXIT_FAILURE;
}



bool find_routing_rec(char * ip, char netmask, struct routing_record * vec, struct routing_record ** result)
{
  char net_ip[MAX_IP];
  gen_net_ip(ip, net_ip, netmask);
  for(size_t i=0; i<vector_size; i++)
  {
    if( strcmp(vec[i].net_ip  ,net_ip) == 0 )
    {
      *result = vec+i;
      return EXIT_SUCCESS;
    }
  }
  return EXIT_FAILURE;
}


bool is_neighbour(struct routing_record * rec)
{
  return strlen(rec->via) == 0;
}

void show_record_routing(struct routing_record r)
{
  printf("net_ip:%s/%u dist:%2u r:%u c_r:%u f:%u c_f:%u n:%u via:%s \n",
    r.net_ip,r.netmask,r.distance, r.reachable,r.rounds_unrechable,
     r.freezd_distance, r.rounds_freezed , r.neighbour ,r.via);
}
void show_record_neighbour(struct neighbour_record r )
{
  printf("my_ip:%s net_ip:%s/%u dist:%u\n",r.my_ip,r.net_ip,r.netmask,r.distance );
}

void update_vector(struct routing_record * vec, struct routing_record * packet)
{
  struct neighbour_record neib_rec;
  memset(&neib_rec, 0,sizeof(struct neighbour_record) );
  //Czy jest z sieci ktore mnie interesuja i nie ejst to pakiet wyslany przezemnie
  if( find_neighbour_rec(packet->via, &neib_rec)  )
  {
    return; //wlasny pakiet lub z niezdefiniowanej sieci
  }
  if(mark_reached(packet->via, vec) )
  {
    //nic sie nie dzieje
    //return;
  }
  //update_net_state

  char new_distance = packet->distance >= INF ? packet->distance : packet->distance + neib_rec.distance;

  struct routing_record * rout_rec;
  if(find_routing_rec(packet->net_ip, packet->netmask, vec, &rout_rec) )
  {
    if(new_distance >= INF)
    {
      return;
    }
    if(vector_size >= MAX_VECTOR_SIZE) {if(TESTING) printf("ERROR: Zbyt duza tablica routingu\n" );return;}
    rout_rec =  vec + vector_size;
    vector_size++;
    packet->distance = new_distance;
    *rout_rec = *packet;
    return;
  }
  //printf("pakiet: ");show_record_routing(*packet);
  //printf("rec_n:  ");show_record_neighbour(neib_rec);
  //printf("rec_r:  ");show_record_routing(*rout_rec);
  if(rout_rec->freezd_distance)
  {//odrzuc info
    return;
  }
  if(new_distance == INF  )
  {
    if(strcmp(rout_rec->via,packet->via) == 0)
    {
      rout_rec->freezd_distance = true;
      rout_rec->rounds_freezed = 0;
      rout_rec->distance = INF;
    }
  }
  if(new_distance < rout_rec->distance)
  {
    rout_rec->distance = new_distance;
    strcpy(rout_rec->via, packet->via);
  }


  //printf("pakiet faktycznie od kogoś:\n" ); show_record_neighbour(neib_rec); show_record_routing(*packet);
  //printf("test poprawy %u+%u na %u\n", packet->distance , neib_rec.distance , rout_rec->distance);

}

void increase_freezd_counter(struct routing_record * vec)
{
  for(size_t i=0; i<vector_size; i++)
  {
    if(vec[i].freezd_distance)
    {
      if(vec[i].rounds_freezed >= LIM_OF_REMOVE )
      {
        if( !vec[i].neighbour)
        {
        vector_delete(vec, i);
        i--;
        }
        else
        {
          vec[i].freezd_distance = false;
        }
      }
      else
      {
          vec[i].rounds_freezed++;
      }
    }
  }
}

void vector_delete(struct routing_record * vec, size_t i)
{
  for(size_t j= i; j < vector_size-1; j++)
  {
    vec[i] = vec[j];
  }
  vector_size--;
}

void copy_neighbour_rec(const struct neighbour_record * src, struct routing_record * dst )
{
  memset(dst,0, sizeof(struct routing_record ) );
  strcpy(dst->net_ip , src->net_ip);
  dst->netmask = src->netmask;
  dst->distance = src->distance;
  dst->reachable =1;
  dst->neighbour = 1;
}

void show_vector(struct routing_record * v)
{
  if(TESTING)
  {
    for(size_t i=0; i<vector_size; i++)
    {
      show_record_routing(v[i]);
    }
    printf("\n" );
    return;
  }
  for(size_t i=0; i<vector_size; i++)
  {
    printf("%s/%u",v[i].net_ip,v[i].netmask);
    if(v[i].distance < INF)
    {
      printf(" distance %u",v[i].distance );
    }
    else
    {
      printf(" unreachable" );
    }
    if(v[i].neighbour )
    {
      printf(" connected directly\n" );
    }
    else
    {
      printf(" via %s\n",v[i].via  );
    }
    //printf("%u d:%u\n",v[i].rounds_nrec_packets, v[i].distance );
  }
  printf("\n" );
}

bool mark_reached(char * ip, struct routing_record * vec)
{
  for (size_t i = 0; i < vector_size; i++)
  {
    if(vec[i].neighbour)
    {
      char net_ip[MAX_IP];
      gen_net_ip(ip, net_ip, vec[i].netmask);
      //printf("kots tu zaglada\n" );
      //printf("ip:%s v:%s",ip,net_ip );
      if(strcmp(net_ip, vec[i].net_ip) == 0 )
      {
        if(!vec[i].reachable)
        {
          vec[i].reachable= true;
          vec[i].rounds_unrechable = 0;
          vec[i].freezd_distance = false;
          vec[i].rounds_freezed = 0;
          struct neighbour_record r;
          find_neighbour_rec(net_ip, &r);
          vec[i].distance = r.distance;
          return 1;
        }
      }
    }
  }
  return 0;
}

void mark_unactive_nets(struct routing_record * vec)
{
  for (size_t i = 0; i < vector_size; i++)
   {
     if(vec[i].neighbour && vec[i].rounds_freezed <= LIM_OF_REMOVE )
     {//gdy wyslane, po prostu zminiamy flaga dla nastepnej rundy
       if(vec[i].reachable)
       {
         vec[i].reachable = false;
       }
       else
       {
         if(vec[i].rounds_unrechable <= LIM_NO_INFO)
         {
           vec[i].rounds_unrechable++;
         }
         else
         {
           vec[i].distance = INF;
           vec[i].freezd_distance = true;
           set_inf(vec[i].net_ip,vec[i].netmask,vec);
         }
       }
     }
  }
}

unsigned int gen_mask(char m)
{
  return (1 << (32-m)) -1;
}

void gen_brodcast_ip(const char * src, char * dst, char mask )
{
  struct in_addr temp;
  inet_pton(AF_INET, src, &temp);
  unsigned int addr = ntohl( temp.s_addr);
  addr = addr | gen_mask(mask);
  temp.s_addr = htonl(addr);
  inet_ntop(AF_INET, &temp.s_addr, dst, MAX_IP);
}

void gen_net_ip(const char * src, char * dst, char mask)
{
  struct in_addr temp;
  inet_pton(AF_INET, src, &temp);
  unsigned int addr = ntohl( temp.s_addr);//printf("%u\n",temp.s_addr );
  addr = addr & ~gen_mask(mask);//printf("%u\n",addr );
  addr = htonl(addr);//printf("%u\n",addr );
  inet_ntop(AF_INET, &addr, dst, MAX_IP);
}
