#include "osc_helper.h"
#include <libxml++/libxml++.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "ommo_midi.h"
#include "ommo_oscdest.h"
#include "ommo_artnetdmx.h"
#include "ommo_lsldest.h"

static int exit_handler(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data)
{
  *((bool*)user_data) = true;
  return 1;
}

typedef std::vector<TASCAR::osc_server_t*> serverlist_t;
typedef std::vector<TASCAR::osc_server_t*>::iterator serverlist_it_t;

typedef std::vector<dest_osc_t*> list_dest_osc_t;
typedef std::vector<dest_osc_t*>::iterator list_dest_osc_it_t;

typedef std::vector<dest_lsl_t*> list_dest_lsl_t;
typedef std::vector<dest_lsl_t*>::iterator list_dest_lsl_it_t;

typedef std::vector<dest_midicc_t*> list_dest_midi_t;
typedef std::vector<dest_midicc_t*>::iterator list_dest_midi_it_t;

typedef std::vector<dest_artnetdmx_t*> list_dest_dmx_t;
typedef std::vector<dest_artnetdmx_t*>::iterator list_dest_dmx_it_t;

void xml_oscdest_data(dest_osc_t* pDestination,xmlpp::Element* TargetElem)
{
  xmlpp::Node::NodeList DataNodes = TargetElem->get_children("data");
  for(xmlpp::Node::NodeList::iterator DataIt=DataNodes.begin();
      DataIt != DataNodes.end(); ++DataIt){
    xmlpp::Element* DataElem = dynamic_cast<xmlpp::Element*>(*DataIt);
    if( DataElem ){
      std::string s_type(DataElem->get_attribute_value("type"));
      std::string s_value(DataElem->get_attribute_value("value"));
      if( (s_type.size() == 1) ){
        switch( s_type[0] ){
        case 'f':
          pDestination->add_float(atof(s_value.c_str()));
          break;
        case 'i':
          pDestination->add_int32(atoi(s_value.c_str()));
          break;
        case 's':
          pDestination->add_string(s_value);
          break;
        }
      }
    }
  }
}

dest_osc_t* xml_oscdest_alloc(xmlpp::Element* TargetElem)
{
  std::string dest(TargetElem->get_attribute_value("dest"));
  std::string path(TargetElem->get_attribute_value("path"));
  std::string s_argmap(TargetElem->get_attribute_value("argmap"));
  dest_osc_t::arg_mode_t argmode(dest_osc_t::source);
  std::vector<unsigned int> argmap;
  if( s_argmap.size() ){
    argmode = dest_osc_t::replace;
    if( strcmp( s_argmap.c_str(), "replace" ) != 0 ){
      argmode = dest_osc_t::reorder;
      std::stringstream ss(s_argmap);
      unsigned int i;
      while (ss >> i){
        argmap.push_back(i);
        if (ss.peek() == ',')
          ss.ignore();
      }
    }
  }
  std::string format(TargetElem->get_attribute_value("printf"));
  if( format.size() )
    argmode = dest_osc_t::printf;
  dest_osc_t* pDestination(new dest_osc_t(dest,path,argmap,argmode,format));
  // now set condition data:
  std::string condition(TargetElem->get_attribute_value("condition"));
  if( condition.size() > 0 )
    pDestination->set_condition(atof(condition.c_str()));
  // value mapping:
  std::string gamma(TargetElem->get_attribute_value("gamma"));
  if( gamma.size() > 0 )
    pDestination->set_gamma(atof(gamma.c_str()));
  std::string s_valmap(TargetElem->get_attribute_value("valmap"));
  if( s_valmap.size() > 0 ){
    std::vector<float> val_map;
    std::stringstream ss(s_valmap);
    float i;
    while (ss >> i){
      val_map.push_back(i);
      if (ss.peek() == ',')
        ss.ignore();
    }
    if( val_map.size() == 2 )
      pDestination->set_valmap( val_map[0], val_map[1] );
  }
  xml_oscdest_data(pDestination,TargetElem);
  return pDestination;
}

int main(int argc, char** argv)
{
  std::string cfgfile;
  if( argc > 1 )
    cfgfile = argv[1];
  else{
    std::cerr << "Usage: " << argv[0] << " <cfgfile> [ <ctlport> ]\n";
    exit(1);
  }
  std::string ctlport("9999");
  if( argc > 2 )
    ctlport = argv[2];
  TASCAR::osc_server_t controller("",ctlport);
  bool b_exit(false);
  controller.add_method("/quit","",exit_handler,&b_exit);
  controller.activate();
  midi_client_t midic("ommo_bridge");
  serverlist_t serverlist;
  list_dest_osc_t list_dest_osc;
  list_dest_lsl_t list_dest_lsl;
  list_dest_midi_t list_dest_midi;
  list_dest_dmx_t list_dest_dmx;
  xmlpp::DomParser parser(cfgfile);
  xmlpp::Element* root = parser.get_document()->get_root_node();
  if( root ){
    // scan entries of OSC server:
    xmlpp::Node::NodeList ServerNodes = root->get_children("oscserver");
    for(xmlpp::Node::NodeList::iterator ServerIt=ServerNodes.begin();
        ServerIt != ServerNodes.end(); ++ServerIt){
      xmlpp::Element* ServerElem = dynamic_cast<xmlpp::Element*>(*ServerIt);
      if( ServerElem ){
        std::string mcaddr(ServerElem->get_attribute_value("multicast"));
        std::string port(ServerElem->get_attribute_value("port"));
        TASCAR::osc_server_t* pServer = new TASCAR::osc_server_t(mcaddr,port);
        // scan DMX targets:
        xmlpp::Node::NodeList DMXNodes = ServerElem->get_children("dmx");
        for(xmlpp::Node::NodeList::iterator TargetIt=DMXNodes.begin();
            TargetIt != DMXNodes.end(); ++TargetIt){
          xmlpp::Element* TargetElem = dynamic_cast<xmlpp::Element*>(*TargetIt);
          if( TargetElem ){
            dest_artnetdmx_t* pDMX(new dest_artnetdmx_t(TargetElem));
            std::string serverpath(TargetElem->get_attribute_value("serverpath"));
            std::string types(TargetElem->get_attribute_value("types"));
            pServer->add_method(serverpath,types.c_str(),dest_artnetdmx_t::static_event_handler,pDMX);
            list_dest_dmx.push_back(pDMX);
          }
        }
        // scan OSC targets:
        xmlpp::Node::NodeList TargetNodes = ServerElem->get_children("osc");
        for(xmlpp::Node::NodeList::iterator TargetIt=TargetNodes.begin();
            TargetIt != TargetNodes.end(); ++TargetIt){
          xmlpp::Element* TargetElem = dynamic_cast<xmlpp::Element*>(*TargetIt);
          if( TargetElem ){
            std::string serverpath(TargetElem->get_attribute_value("serverpath"));
            std::string types(TargetElem->get_attribute_value("types"));
            dest_osc_t* pDestination(xml_oscdest_alloc(TargetElem));
            pServer->add_method(serverpath,types.c_str(),dest_osc_t::static_event_handler,pDestination);
            list_dest_osc.push_back(pDestination);
          }
        }
        // scan LSL targets:
        xmlpp::Node::NodeList TargetNodesLSL = ServerElem->get_children("lsl");
        for(xmlpp::Node::NodeList::iterator TargetIt=TargetNodesLSL.begin();
            TargetIt != TargetNodesLSL.end(); ++TargetIt){
          xmlpp::Element* TargetElem = dynamic_cast<xmlpp::Element*>(*TargetIt);
          if( TargetElem ){
            std::string serverpath(TargetElem->get_attribute_value("serverpath"));
            std::string types(TargetElem->get_attribute_value("types"));
            dest_lsl_t* pDestination( new dest_lsl_t( TargetElem, types.size() ) );
            pServer->add_method(serverpath,types.c_str(),dest_lsl_t::static_event_handler,pDestination);
            list_dest_lsl.push_back(pDestination);
          }
        }
        // scan midi targets:
        xmlpp::Node::NodeList MidiNodes = ServerElem->get_children("midicc");
        for(xmlpp::Node::NodeList::iterator MidiIt=MidiNodes.begin();
            MidiIt != MidiNodes.end(); ++MidiIt){
          xmlpp::Element* MidiElem = dynamic_cast<xmlpp::Element*>(*MidiIt);
          if( MidiElem ){
            std::string s_channel(MidiElem->get_attribute_value("channel"));
            int channel(atoi(s_channel.c_str()));
            std::string s_param(MidiElem->get_attribute_value("param"));
            int param(atoi(s_param.c_str()));
            std::string s_argnum(MidiElem->get_attribute_value("argnum"));
            int argnum(atoi(s_argnum.c_str()));
            std::string serverpath(MidiElem->get_attribute_value("serverpath"));
            std::string types(MidiElem->get_attribute_value("types"));
            dest_midicc_t* pMidi;
            pMidi = new dest_midicc_t(midic,channel,param,argnum);
            pServer->add_method(serverpath,types.c_str(),dest_midicc_t::static_event_handler,pMidi);
            list_dest_midi.push_back(pMidi);
          }
        }
        serverlist.push_back(pServer);
      }
    }
    // scan entries of MIDI server:
    xmlpp::Node::NodeList MidiRecNodes = root->get_children("midireceiver");
    for(xmlpp::Node::NodeList::iterator MidiRecIt=MidiRecNodes.begin();
        MidiRecIt != MidiRecNodes.end(); ++MidiRecIt){
      xmlpp::Element* ServerElem = dynamic_cast<xmlpp::Element*>(*MidiRecIt);
      if( ServerElem ){
        // scan OSC targets:
        xmlpp::Node::NodeList TargetNodes = ServerElem->get_children("osc");
        for(xmlpp::Node::NodeList::iterator TargetIt=TargetNodes.begin();
            TargetIt != TargetNodes.end(); ++TargetIt){
          xmlpp::Element* TargetElem = dynamic_cast<xmlpp::Element*>(*TargetIt);
          if( TargetElem ){
            std::string s_cchannel(TargetElem->get_attribute_value("clientchannel"));
            int cchannel(atoi(s_cchannel.c_str()));
            std::string s_cparam(TargetElem->get_attribute_value("clientparam"));
            int cparam(atoi(s_cparam.c_str()));
            dest_osc_t* pDestination(xml_oscdest_alloc(TargetElem));
            midic.add_method(cchannel,cparam,dest_osc_t::static_event_handler,pDestination);
            list_dest_osc.push_back(pDestination);
          }
        }
        // scan LSL targets:
        xmlpp::Node::NodeList TargetNodesLSL = ServerElem->get_children("lsl");
        for(xmlpp::Node::NodeList::iterator TargetIt=TargetNodesLSL.begin();
            TargetIt != TargetNodesLSL.end(); ++TargetIt){
          xmlpp::Element* TargetElem = dynamic_cast<xmlpp::Element*>(*TargetIt);
          if( TargetElem ){
            std::string s_cchannel(TargetElem->get_attribute_value("clientchannel"));
            int cchannel(atoi(s_cchannel.c_str()));
            std::string s_cparam(TargetElem->get_attribute_value("clientparam"));
            int cparam(atoi(s_cparam.c_str()));
            dest_lsl_t* pDestination( new dest_lsl_t( TargetElem, 1 ) );
            midic.add_method(cchannel,cparam,dest_lsl_t::static_event_handler,pDestination);
            list_dest_lsl.push_back(pDestination);
          }
        }
        // scan midi targets:
        xmlpp::Node::NodeList MidiNodes = ServerElem->get_children("midicc");
        for(xmlpp::Node::NodeList::iterator MidiIt=MidiNodes.begin();
            MidiIt != MidiNodes.end(); ++MidiIt){
          xmlpp::Element* MidiElem = dynamic_cast<xmlpp::Element*>(*MidiIt);
          if( MidiElem ){
            std::string s_cchannel(MidiElem->get_attribute_value("clientchannel"));
            int cchannel(atoi(s_cchannel.c_str()));
            std::string s_cparam(MidiElem->get_attribute_value("clientparam"));
            int cparam(atoi(s_cparam.c_str()));
            std::string s_channel(MidiElem->get_attribute_value("channel"));
            int channel(atoi(s_channel.c_str()));
            std::string s_param(MidiElem->get_attribute_value("param"));
            int param(atoi(s_param.c_str()));
            dest_midicc_t* pMidi;
            pMidi = new dest_midicc_t(midic,channel,param,0);
            midic.add_method(cchannel,cparam,dest_midicc_t::static_event_handler,pMidi);
            list_dest_midi.push_back(pMidi);
          }
        }
      }
    }
  }
  for( serverlist_it_t srv=serverlist.begin();srv!=serverlist.end();++srv)
    (*srv)->activate();
  midic.start_service();
  while(!b_exit){
    usleep(1000);
  }
  midic.stop_service();
  for( serverlist_it_t srv=serverlist.begin();srv!=serverlist.end();++srv)
    (*srv)->deactivate();
  controller.deactivate();
  // clean up:
  for( serverlist_it_t srv=serverlist.begin();srv!=serverlist.end();++srv)
    delete (*srv);
  for( list_dest_midi_it_t it=list_dest_midi.begin();it!=list_dest_midi.end();++it)
    delete (*it);
  for( list_dest_osc_it_t it=list_dest_osc.begin();it!=list_dest_osc.end();++it)
    delete (*it);
  for( list_dest_dmx_it_t it=list_dest_dmx.begin();it!=list_dest_dmx.end();++it)
    delete (*it);
  return 0;
}

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
