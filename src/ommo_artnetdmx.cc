#include "ommo_artnetdmx.h"

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


dest_artnetdmx_t::dest_artnetdmx_t(xmlpp::Element* cfg)
  : artnet_cfg_t(cfg),
    artnetDMX_t(hostname.c_str(),port.c_str())
{
  prepare();
}

void dest_artnetdmx_t::prepare()
{
  data.resize(channels.size());
  for(uint32_t k=scale.size();k<channels.size();++k)
    scale.push_back(1.0f);
}

int dest_artnetdmx_t::event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg)
{
  if( (strcmp(types,"f")==0) && (argc==1) ){
    for(uint32_t k=0;k<channels.size();++k)
      data[k] = (channels[k] << 8) + std::min(255.0f,std::max(0.0f,(float)(scale[k])*(argv[0]->f)));
    send(universe,data);
  }
  return 1;
}

artnet_cfg_t::artnet_cfg_t(xmlpp::Element* cfg)
  : xml_element_t(cfg)
{
  GET_ATTRIBUTE(hostname);
  GET_ATTRIBUTE(port);
  GET_ATTRIBUTE(universe);
  GET_ATTRIBUTE(channels);
  GET_ATTRIBUTE(scale);
}

dest_artnetdmx_t::~dest_artnetdmx_t()
{
}

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
