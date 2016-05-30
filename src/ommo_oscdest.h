#ifndef OMMO_OSCDEST_H
#define OMMO_OSCDEST_H

#include "ommo_dest.h"

class osc_destination_t : public dest_t {
public:
  enum arg_mode_t {
    source, replace, reorder, printf
  };
  osc_destination_t(const std::string& target,const std::string& path, const std::vector<unsigned int>& argmap, arg_mode_t argmode, const std::string& format);
  virtual ~osc_destination_t();
  int event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message in_msg);
  void set_valmap(float v1, float v2);
  void set_gamma(float g) {gamma = g;};
  void add_float(float f);
  void add_int32(int32_t f);
  void add_string(const std::string& s);
  void set_condition(float val);
  void remove_condition();
  bool lo_message_copy_arg(lo_message dest, lo_message src, int arg, bool x_valmap);
private:
  lo_address target_;
  std::string path_;
  std::vector<unsigned int> argmap_;
  arg_mode_t argmode_;
  std::string format_;
  lo_message own_msg;
  bool use_condition;
  float condition_val;
  bool use_valmap;
  float val_min;
  float val_scale;
  float gamma;
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
