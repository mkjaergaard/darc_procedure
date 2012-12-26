#include <darc/procedure/procedure_service.hpp>

namespace darc
{
namespace procedure
{

procedure_service::procedure_service(peer& p, boost::asio::io_service& io_service, ns_service& ns_service) :
  peer_service(p, 26),
  io_service_(io_service),
  nameserver_(ns_service)
{

}

}
}
