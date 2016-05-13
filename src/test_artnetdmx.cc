#include "artnet.h"

int main(int argc, char** argv)
{
  artnetDMX_t dmx("192.168.0.9","6454");
  std::vector<uint16_t> data;
  data.resize(1);
  data[0] = 0x01ff;
  dmx.send(1,data);
}

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
