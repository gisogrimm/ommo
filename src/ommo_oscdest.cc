#include "ommo_oscdest.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

void osc_destination_t::set_valmap(float v1, float v2)
{
  val_min = v1;
  //val_scale = std::min(1.0,std::max(0.0,1.0/(v2-v1)));
  val_scale = (v2-v1);
  use_valmap = true;
}

void osc_destination_t::set_condition(float val)
{
  use_condition = true;
  condition_val = val;
}

void osc_destination_t::remove_condition()
{
  use_condition = false;
}

void osc_destination_t::add_float(float f)
{
  lo_message_add_float(own_msg,f);
}

void osc_destination_t::add_int32(int32_t i)
{
  lo_message_add_float(own_msg,i);
}

void osc_destination_t::add_string(const std::string& s)
{
  lo_message_add_string(own_msg,s.c_str());
}

bool osc_destination_t::lo_message_copy_arg(lo_message dest, lo_message src, int arg, bool x_valmap)
{
  int argc(lo_message_get_argc(src));
  const char* types(lo_message_get_types(src));
  lo_arg** argv(lo_message_get_argv(src));
  float val;
  if( (arg >= 0) && (arg < argc) ){
    switch( types[arg] ){
    case 'i' :
      lo_message_add_int32(dest,argv[arg]->i);
      return true;
    case 'f' :
      val = argv[arg]->f;
      if( use_valmap && x_valmap )
        val = val_scale*val+val_min;
      lo_message_add_float(dest,val);
      return true;
    case 's' :
      lo_message_add_string(dest,&(argv[arg]->s));
      return true;
    }
  }
  return false;
}

osc_destination_t::osc_destination_t(const std::string& target,const std::string& path, const std::vector<unsigned int>& argmap, arg_mode_t argmode, const std::string& format)
  : target_(lo_address_new_from_url(target.c_str())),
    path_(path),
    argmap_(argmap),
    argmode_(argmode),
    format_(format),
    own_msg(lo_message_new()),
    use_condition(false),
    condition_val(0),
    use_valmap(false),
    gamma(1.0)
{
  lo_address_set_ttl(target_,1);
}

osc_destination_t::~osc_destination_t()
{
  lo_message_free(own_msg);
  lo_address_free(target_);
}

int osc_destination_t::event_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message in_msg)
{
  if( use_condition ){
    if( (argc > 0) && (types[0] == 'f') )
      if( (argv[0]->f) != condition_val )
        return 1;
  }
  switch( argmode_ ){
  case source :
    {
      lo_send_message( target_, path_.c_str(), in_msg );
    }
    break;
  case replace :
    {
      lo_send_message( target_, path_.c_str(), own_msg );
    }
    break;
  case reorder :
    {
      lo_message msg(lo_message_new());
      int own_msg_arg_index(0);
      for(unsigned int k=0;k<argmap_.size();k++){
        if( (argmap_[k]) == 0 ){
          lo_message_copy_arg( msg, own_msg, own_msg_arg_index, false );
          own_msg_arg_index++;
        }else{
          lo_message_copy_arg( msg, in_msg, argmap_[k]-1, true );
        }
      }
      lo_send_message( target_, path_.c_str(), msg );
      lo_message_free(msg);
    }
    break;
  case printf :
    {
      if( (strcmp(types,"f")==0) && (argc==1) ){
        float val(pow(argv[0]->f,gamma));
        if( use_valmap )
          val = val_scale*val+val_min;
        char ctmp[1024];
        ctmp[1023] = 0;
        snprintf(ctmp,1023,format_.c_str(),val);
        lo_send( target_, path_.c_str(), "s", ctmp );
      }
    }
    break;
  }
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
