#include "ommo_midi.h"

#define MIDISC 0.0078740157480314959537182f

midi_client_t::handler_t::handler_t(uint8_t ch,uint8_t par,lo_method_handler lh,void* data)
  : lh_(lh),data_(data),ch_(ch),par_(par)
{
}

void midi_client_t::handler_t::emit(uint8_t ch,uint8_t par,uint8_t value)
{
  if( (ch==ch_) && (par==par_) ){
    // emit message
    lo_message msg(lo_message_new());
    lo_message_add_float(msg,MIDISC*value);
    lh_("/midicc",lo_message_get_types(msg),lo_message_get_argv(msg),lo_message_get_argc(msg),msg,data_);
    lo_message_free(msg);
  }
}

void midi_client_t::add_method(uint8_t channel, uint8_t param, lo_method_handler lh, void* data)
{
  vH.push_back(midi_client_t::handler_t(channel,param,lh,data));
}

midi_client_t::midi_client_t(const std::string& name)
  : midi_ctl_t(name)
{
}

midi_client_t::~midi_client_t()
{
}

void midi_client_t::emit_event(int channel, int param, int value)
{
  for(std::vector<midi_client_t::handler_t>::iterator it=vH.begin();it!=vH.end();++it)
    it->emit(channel,param,value);
}



dest_midicc_t::dest_midicc_t(midi_client_t& mc, unsigned int channel, unsigned int param, unsigned argnum)
  : mc_(mc),
    channel_(channel),
    param_(param),
    argnum_(argnum)
{
}

dest_midicc_t::~dest_midicc_t()
{
}

int dest_midicc_t::event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message in_msg)
{
  if( ((int)(argnum_) < argc) && (types[argnum_] == 'f') )
    mc_.send_midi(channel_,param_,127*(argv[argnum_]->f));
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
