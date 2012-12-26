#pragma once

#include <boost/asio.hpp>
#include <darc/procedure/client.hpp>
#include <darc/procedure/server.hpp>
#include <darc/tag_handle.hpp>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Result, typename T_Feedback>
class local_dispatcher
{
protected:
  typedef client_impl<T_Arg, T_Result, T_Feedback> ClientType;
  typedef server_impl<T_Arg, T_Result, T_Feedback> ServerType;

  typedef std::vector<ClientType*> ClientListType;
  typedef std::map</*call*/ID, NodeID> ActiveServerCallsType;
  typedef std::map</*call*/ID, ClientType*> ActiveClientCallsType;

  //dispatcher_group<T> * group_;

  ServerType* server_;
  ClientListType client_list_;

  // Structure to hold procedure calls we have received
  ActiveServerCallsType active_server_calls_;

  // Structure to hold procedure calls we have made
  ActiveClientCallsType active_client_calls_;

  tag_handle tag_;

public:
  local_dispatcher(const tag_handle& tag) :
    tag_(tag)
  {
  }

  void check_empty()
  {
    assert(false);
  }

  void attach(ClientType &client)
  {
    client_list_.push_back(&client);
  }

  void attach(ServerType &server)
  {
    assert(server_ == 0);
    server_ = &server;
  }

  void detach(client_impl<T> &client)
  {
  }

  void detach(server_impl<T> &client)
  {
  }

  void dispatch_call_locally(const ID& call_id,
			     const boost::shared_ptr<const T_Arg> &arg)
  {
    if(server_ != 0)
    {
      active_server_calls_.insert(ActiveServerCallsType::value_type(call_id));
      server_->postCall(call_id, arg);
    }
  }

  void call(ClientType* client, const boost::shared_ptr<const T_Arg> &arg)
  {
    ID call_id = ID::create();
    active_client_calls_.insert(ActiveClientCallsType::value_type(call_id
								  client));
    dispatch_call_locally(call_id, arg);
  }

};

}
}
