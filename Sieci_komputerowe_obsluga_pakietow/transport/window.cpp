// Kacper Kulczak 279079
#include "window.h"

Window::Window(size_t file_length)
{
  this->file_len = file_length;
  this->bytes_recived = 0;
}

void Window::init(int port, std::string file_name)
{
  // socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
  {
    throw std::string("Socket error: ") + strerror(errno);
  }
  // setting adress struct
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  inet_pton(AF_INET,"156.17.4.30", &server_address.sin_addr);
  // setting descriptors for select
  // FD_ZERO(&descriptors);
  // FD_SET(sockfd, &descriptors);
  //opening file
  fout.open(file_name, std::fstream::out | std::fstream::binary);
}

void Window::gen_win()
{
  for(size_t i=0; i < WINDOW_SIZE; i++)
  {
    if( should_increase_window() )
    {
      add_part();
    }
    else
    {
      #ifdef DEBUG
        std::cerr << " Window gen interrupt. \n";
      #endif
      return;
    }
  }
}

void Window::add_part()
{
  size_t part_len = 1000;
  if(bytes_next + part_len > file_len)
  {
    part_len = file_len - bytes_next;
  }
  win.push_back(Part(bytes_next, part_len, sockfd, &server_address));
  bytes_next += part_len;
}

bool Window::recive_packet()
{
  struct sockaddr_in sender;
  socklen_t sock_len = sizeof(sender);
  ssize_t datagram_len = recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr *)&sender, &sock_len);
  if(datagram_len < 0)
  {
    throw std::string("Recvfrom error") + strerror(errno);
    return EXIT_FAILURE;
  }
  if( server_address.sin_port == sender.sin_port &&
      server_address.sin_addr.s_addr == sender.sin_addr.s_addr)
  {
    if( sscanf((char *)buffer, "DATA %zu %zu\n",&recv_start, &recv_len) == 2 )
    {
      recv_offset = 7 + std::to_string(recv_start).size() + std::to_string(recv_len).size();
      return EXIT_SUCCESS;
    }
  }
  #ifndef DEBUG
    std::cerr << "Packet from somwhere else!!!\n";
  #endif
  return EXIT_FAILURE;
}

void Window::download_file()
{

  gen_win();
  send_requests();
  struct timeval tv = {0,25000};
  for(;;)
    {
      #ifdef DEBUG
        std::cerr << tv.tv_sec << " " << tv.tv_usec << "\n";
      #endif
      fd_set descriptors;
      FD_ZERO(&descriptors);
      FD_SET(sockfd, &descriptors);
      int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
      if( ready < 0)
      {
        throw std::string("Select error: ") + strerror(errno);
      }
      else if( ready == 0)
      {
        send_requests();
        tv = {0,TIME_TO_SEND};
      }
       if(ready > 0 )
      {
        #ifdef DEBUG
          std::cerr << "Packet ready to recive\n";
        #endif
        recive_packet();
        find_propiert_part();
        shift();
        if( win.empty() )
        {
          fout.close();
          return;
        }
      }
      // std::cerr << win.front().getdata().size() << "\n";
      // fout << win.front().getdata();
    }
  }

bool Window::find_propiert_part()
{
  for(auto i=win.begin(); i != win.end() ; i++ )
  {
    if(i->getstart() == recv_start && i->getlen() == recv_len)
    {
      if(!i->is_recived())
      {
        i->recive_data((char *)buffer + recv_offset );
        i->part_recived();
        return EXIT_SUCCESS;
      }
    }
  }
  return EXIT_FAILURE;
}

void Window::shift()
{
  while( !win.empty() &&  win.front().is_recived() )
  {
    bytes_recived += win.front().getlen();
    std::cout << std::fixed << std::setprecision(2) << (double)bytes_recived / (double)file_len * 100.0 << "% done.\n";
    for(size_t i = 0; i <win.front().getlen(); i++  )
    {
      fout <<  win.front().getdata()[i] ;
    }

    win.pop_front();
    if( should_increase_window() )
    {
      add_part();
    }
  }
}

void Window::send_requests()
{
  for(auto i=win.begin(); i != win.end(); i++)
  {
    if( !i->is_recived() )
    {
      i->send_request();
    }
  }
}
