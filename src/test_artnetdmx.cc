#include "ommo_artnetdmx.h"

int main(int argc, char** argv)
{
  artnetDMX_t dmx("192.168.0.9","6454");
  std::vector<uint16_t> data;
  data.resize(32);
  for(uint32_t k=0;k<32;k++)
    data[k] = 0x01ff;
  dmx.send(0,data);
}

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
