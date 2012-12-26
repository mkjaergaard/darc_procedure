#pragma once

#include <boost/asio/io_service.hpp>
#include <darc/peer.hpp>
#include <darc/peer_service.hpp>
#include <darc/ns_service.hpp>

namespace darc
{
namespace procedure
{

// fwd
template<typename Arg, typename Result, typename Feedback>
class client_impl;

template<typename Arg, typename Result, typename Feedback>
class server_impl;

template<typename Arg, typename Result, typename Feedback>
class local_dispatcher;

// class
class procedure_service : public boost::noncopyable, public darc::peer_service
{
protected:
  boost::asio::io_service& io_service_;
  ns_service& nameserver_;

public:
  procedure_service(peer& p, boost::asio::io_service& io_service, ns_service& ns_service);

protected:
  void recv(const darc::ID& src_peer_id,
            darc::buffer::shared_buffer data)
  {
  }

  template<typename Arg, typename Result, typename Feedback>
  local_dispatcher<Arg, Result, Feedback>* attach(client_impl<Arg, Result, Feedback>& client,
						  const std::string& tag)
  {
    return 0;
  }

  template<typename Arg, typename Result, typename Feedback>
  void detach(client_impl<Arg, Result, Feedback>& client)
  {
  }

  template<typename Arg, typename Result, typename Feedback>
  local_dispatcher<Arg, Result, Feedback>* attach(server_impl<Arg, Result, Feedback>& client,
						  const std::string& tag)
  {
    return 0;
  }

  template<typename Arg, typename Result, typename Feedback>
  void detach(server_impl<Arg, Result, Feedback>& client)
  {
  }


};

}
}
