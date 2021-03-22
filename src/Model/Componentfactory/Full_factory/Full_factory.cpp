#include "Full_factory.hpp"

#include <Gas_factory.hpp>
#include <Power_factory.hpp>

#include "Compressorstation.hpp"
#include "Controlvalve.hpp"
#include "Gaspowerconnection.hpp"
#include "Innode.hpp"
#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "Sink.hpp"
#include "Source.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

namespace Model::Componentfactory {

  void add_all_components(Componentfactory &factory){
    add_power_components(factory);
    add_gas_components(factory);
  }

  Full_factory::Full_factory(){
    add_power_components(*this);
    add_gas_components(*this);
  }
}
