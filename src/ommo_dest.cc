#include "ommo_dest.h"

dest_t::dest_t() 
{
}

int dest_t::static_event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data)
{
  return ((dest_t*)user_data)->event_handler(path,types,argv,argc,msg);
}

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
