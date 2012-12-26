#include <gtest/gtest.h>

#include <darc/test/two_peer_sim.hpp>

#include <darc/procedure/procedure_service.hpp>
#include <darc/procedure/client.hpp>
#include <darc/procedure/server.hpp>
#include <darc/procedure/local_dispatcher.hpp>

class PubSubTest : public darc::test::two_peer_sim, public testing::Test
{
public:
  darc::distributed_container::container_manager mngr1;
  darc::distributed_container::container_manager mngr2;
  darc::ns_service ns1;
  darc::ns_service ns2;

  PubSubTest() :
    mngr1(peer1),
    mngr2(peer2),
    ns1(peer1, &mngr1),
    ns2(peer2, &mngr2)
  {
  }
};

void call_handler(const darc::ID& call_id, const boost::shared_ptr<const int>& arg)
{
  beam::glog<beam::Info>("Procedure Call!!!!!");
}

TEST_F(PubSubTest, PubSub)
{
  boost::asio::io_service io_service;
  darc::procedure::procedure_service service(peer1, io_service, ns1);

  darc::procedure::client<int, int, int> my_client(io_service, service);
  darc::procedure::server<int, int, int> my_server(io_service, service);

  my_client.attach("blip");
  my_server.attach("blip");

  my_server.set_call_handler(boost::bind(&call_handler, _1, _2));

  boost::shared_ptr<int> value(new int(5));
  darc::ID call_id = my_client.call(value);

  io_service.run();
}
