#ifndef OMMO_MIDI_H
#define OMMO_MIDI_H

#include "libhos_midi_ctl.h"
#include <stdint.h>
#include "ommo_dest.h"

class midi_client_t : public midi_ctl_t {
public:
  midi_client_t(const std::string& name);
  ~midi_client_t();
  void add_method(uint8_t channel, uint8_t param, lo_method_handler lh, void* data);
protected:
  void emit_event(int channel, int param, int value);
private:
  class handler_t {
  public:
    handler_t(uint8_t ch,uint8_t par,lo_method_handler lh,void* data);
    void emit(uint8_t ch,uint8_t par,uint8_t value);
  private:
    lo_method_handler lh_;
    void* data_;
    uint8_t ch_;
    uint8_t par_;
  };
  std::vector<handler_t> vH;
};

class midi_destination_t : public dest_t {
public:
  midi_destination_t(midi_client_t& mc, unsigned int channel, unsigned int param, unsigned argnum);
  ~midi_destination_t();
  //static int event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data);
  int event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message in_msg);
private:
  midi_client_t& mc_;
  unsigned int channel_;
  unsigned int param_;
  unsigned int argnum_;
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
