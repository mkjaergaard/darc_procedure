#pragma once

#include <boost/asio.hpp>
#include <darc/procedure/local_dispatcher_base.hpp>
#include <darc/procedure/client.hpp>
#include <darc/procedure/server.hpp>
#include <darc/tag_handle.hpp>

namespace darc
{
namespace procedure
{

template<typename T_Arg, typename T_Result, typename T_Feedback>
class local_dispatcher : public local_dispatcher_base
{
protected:
  typedef client_impl<T_Arg, T_Result, T_Feedback> ClientType;
  typedef server_impl<T_Arg, T_Result, T_Feedback> ServerType;

  typedef std::vector<ClientType*> ClientListType;
  typedef std::map</*call*/ID, ClientType*> ActiveClientCallsType;

  //dispatcher_group<T> * group_;

  ServerType* server_;
  ClientListType client_list_;

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

  void detach(ClientType &client)
  {
  }

  void detach(ServerType &server)
  {
  }

  void dispatch_call_locally(const ID& call_id,
			     const boost::shared_ptr<const T_Arg> &arg)
  {
    if(server_ != 0)
    {
      server_->post_call(call_id, arg);
    }
  }

  ID call_from_local(ClientType* client, const boost::shared_ptr<const T_Arg> &arg)
  {
    ID call_id = ID::create();
    active_client_calls_.insert(typename ActiveClientCallsType::value_type(call_id,
									   client));
    dispatch_call_locally(call_id, arg);
    return call_id;
  }

  void feedback_from_local(const ID& call_id, const boost::shared_ptr<const T_Feedback> &feedback_msg)
  {
    typename ActiveClientCallsType::iterator item =
      active_client_calls_.find(call_id);

    if(item == active_client_calls_.end())
    {
      beam::glog<beam::Warning>("Feedback for unknown call_id");
      return;
    }
    else
    {
      item->second->post_feedback(call_id, feedback_msg);
    }
  }

  void result_from_local(const ID& call_id, const boost::shared_ptr<const T_Feedback> &result_msg)
  {
    typename ActiveClientCallsType::iterator item =
      active_client_calls_.find(call_id);

    if(item == active_client_calls_.end())
    {
      beam::glog<beam::Warning>("Result for unknown call_id");
      return;
    }
    else
    {
      item->second->post_result(call_id, result_msg);
      active_client_calls_.erase(item);
    }
  }


};

}
}
