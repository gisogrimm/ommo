#ifndef OMMO_DEST_H
#define OMMO_DEST_H

#include <lo/lo.h>
#include <string>
#include <vector>

class dest_t {
public:
  dest_t();
  static int static_event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data); 
  virtual int event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg) = 0;
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
