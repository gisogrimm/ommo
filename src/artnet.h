#ifndef ARTNET_H
#define ARTNET_H

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>

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

#endif


/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
