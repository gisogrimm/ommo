#include "ommo_lsldest.h"

lsl_cfg_t::lsl_cfg_t( xmlpp::Element* cfg )
  : xml_element_t(cfg)
{
  GET_ATTRIBUTE(name);
  GET_ATTRIBUTE(type);
}

lsl_destination_t::lsl_destination_t( xmlpp::Element* cfg, uint32_t lslchannels )
  : lsl_cfg_t( cfg ),
    info( name, type, lslchannels ),
    outlet(info),
    lslchannels_(lslchannels)
{
}

lsl_destination_t::~lsl_destination_t()
{
}

int lsl_destination_t::event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message in_msg)
{
  if( argc != (int)lslchannels_ )
    return 1;
  for(uint32_t k=0;k<lslchannels_;++k)
    if( types[k] != 'f' )
      return 1;
  float data[lslchannels_];
  for(uint32_t k=0;k<lslchannels_;++k)
    data[k] = argv[k]->f;
  outlet.push_sample(data);
  return 1;
}

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
