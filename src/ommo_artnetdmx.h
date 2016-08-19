#ifndef OMMO_ARTNETDMX_H
#define OMMO_ARTNETDMX_H

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>

#include "ommo_dest.h"
#include <xmlconfig.h>

class artnetDMX_t {
public:
  artnetDMX_t(const char* hostname, const char* port);
  void send(uint8_t universe, const std::vector<uint16_t>& data);
private:
  uint8_t msg[530];
  uint8_t* data_;
  struct addrinfo* res;
  int fd;
};

class artnet_cfg_t : public TASCAR::xml_element_t {
public:
  artnet_cfg_t(xmlpp::Element* cfg);
  std::string hostname;
  std::string port;
  uint32_t universe;
  std::vector<int32_t> channels;
  std::vector<double> scale;
};

class dest_artnetdmx_t : public dest_t, public artnet_cfg_t, public artnetDMX_t {
public:
  dest_artnetdmx_t(xmlpp::Element* cfg);
  virtual ~dest_artnetdmx_t();
  void write_xml() {};
  virtual int event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg);
  void prepare();
private:
  std::vector<uint16_t> data;
};

#endif


/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
