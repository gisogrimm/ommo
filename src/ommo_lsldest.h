#ifndef OMMO_LSLDEST_H
#define OMMO_LSLDEST_H

#include <lsl_cpp.h>
#include "ommo_dest.h"
#include <xmlconfig.h>

class lsl_cfg_t : public TASCAR::xml_element_t {
public:
  lsl_cfg_t(xmlpp::Element* cfg);
  void write_xml() {};
  std::string name;
  std::string type;
};

class lsl_destination_t : public dest_t, public lsl_cfg_t {
public:
  lsl_destination_t( xmlpp::Element* cfg, uint32_t lslchannels );
  virtual ~lsl_destination_t();
  int event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message in_msg);
private:
  lsl::stream_info info;
  lsl::stream_outlet outlet;
  uint32_t lslchannels_;
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
