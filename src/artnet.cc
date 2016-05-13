#include "artnet.h"

artnetDMX_t::artnetDMX_t(const char* hostname, const char* port)
  : data_(&(msg[18])),
    res(0)
{
  memset(msg,0,530);
  msg[0] = 'A';
  msg[1] = 'r';
  msg[2] = 't';
  msg[3] = '-';
  msg[4] = 'N';
  msg[5] = 'e';
  msg[6] = 't';
  msg[9] = 0x50;
  msg[11] = 14;
  struct addrinfo hints;
  memset(&hints,0,sizeof(hints));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_DGRAM;
  hints.ai_protocol=0;
  hints.ai_flags=AI_ADDRCONFIG;
  int err(getaddrinfo( hostname, port, &hints, &res ));
  if(err != 0)
    throw "failed to resolve remote socket address";
  fd = socket( res->ai_family, res->ai_socktype, res->ai_protocol );
  if (fd == -1)
    throw strerror(errno);
}

void artnetDMX_t::send(uint8_t universe, const std::vector<uint16_t>& data)
{
  msg[14] = universe;
  msg[16] = data.size() >> 8;
  msg[17] = data.size() & 0xff;
  for(uint32_t k=0;k<data.size();++k)
    data_[k] = data[k];
  if( sendto(fd,msg,18+data.size(),0,res->ai_addr,res->ai_addrlen) == -1 )
    throw strerror(errno);
}

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
