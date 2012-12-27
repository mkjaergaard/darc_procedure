#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/thread/mutex.hpp>
#include <darc/peer.hpp>
#include <darc/peer_service.hpp>
#include <darc/ns_service.hpp>
#include <darc/procedure/local_dispatcher_base.hpp>

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

  typedef std::map<darc::ID, boost::shared_ptr<local_dispatcher_base> > dispatcher_list_type;
  dispatcher_list_type dispatcher_list_;

  // Temporary solution to protect multi-thread access of dispatchers
  boost::mutex mutex_;

public:
  procedure_service(peer& p, boost::asio::io_service& io_service, ns_service& ns_service);

protected:
  void recv(const darc::ID& src_peer_id,
            darc::buffer::shared_buffer data)
  {
  }

public:
  template<typename Arg, typename Result, typename Feedback>
  local_dispatcher<Arg, Result, Feedback>* attach(client_impl<Arg, Result, Feedback>& client,
                                                  const std::string& tag)
  {
    boost::mutex::scoped_lock lock(mutex_);
    tag_handle handle = nameserver_.register_tag(nameserver_.root(), tag);
    local_dispatcher<Arg, Feedback, Result>* dispatcher = get_dispatcher<Arg, Result, Feedback>(handle);
    dispatcher->attach(client);
    return dispatcher;
  }

  template<typename Arg, typename Result, typename Feedback>
  void detach(client_impl<Arg, Result, Feedback>& client)
  {
  }

  template<typename Arg, typename Result, typename Feedback>
  local_dispatcher<Arg, Result, Feedback>* attach(server_impl<Arg, Result, Feedback>& server,
						  const std::string& tag)
  {
    boost::mutex::scoped_lock lock(mutex_);
    tag_handle handle = nameserver_.register_tag(nameserver_.root(), tag);
    local_dispatcher<Arg, Result, Feedback>* dispatcher = get_dispatcher<Arg, Result, Feedback>(handle);
    dispatcher->attach(server);
    return dispatcher;
  }

  template<typename Arg, typename Result, typename Feedback>
  void detach(server_impl<Arg, Result, Feedback>& client)
  {
  }

  template<typename Argument, typename Result, typename Feedback>
  local_dispatcher<Argument, Result, Feedback>*
  get_dispatcher(const tag_handle& tag);

};

}
}
