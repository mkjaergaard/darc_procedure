#pragma once

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/signals.hpp>

#include <darc/procedure/procedure_service.hpp>
#include <darc/procedure/local_dispatcher__fwd.hpp>

#include <darc/id.hpp>

#include <boost/make_shared.hpp>

namespace darc
{
namespace procedure
{

template<typename Argument, typename Feedback, typename Result>
class server_impl
{
public:
  typedef void(call_type)(const ID&, const boost::shared_ptr<const Argument>&);
  typedef boost::function<call_type> call_functor_type;

private:
  boost::asio::io_service &io_service_;
  procedure_service &procedure_service_;

  local_dispatcher<Argument, Feedback, Result> * dispatcher_;
  call_functor_type call_handler_;

public:
  server_impl(boost::asio::io_service &io_service,
	      procedure_service &procedure_service) :
    io_service_(io_service),
    procedure_service_(procedure_service),
    dispatcher_(0)
  {
  }

  void attach(const std::string& tag)
  {
    dispatcher_ = procedure_service_.template attach<Argument, Feedback, Result>(*this, tag);
  }

  void detach()
  {
    if(dispatcher_ != 0)
    {
      procedure_service_.detach(*this, dispatcher_);
      dispatcher_ = 0;
    }
  }

  void set_call_handler(call_functor_type handler)
  {
    call_handler_ = handler;
  }

  void post_call(const ID& call_id, const boost::shared_ptr<const Argument> &arg)
  {
    io_service_.post(boost::bind(&server_impl::trigger_call, this, call_id, arg));
  }

  void trigger_call(const ID& call_id, const boost::shared_ptr<const Argument> &arg)
  {
    call_handler_(call_id, arg);
  }

};

template<typename Argument, typename Feedback, typename Result>
class server
{
public:
  typedef typename server_impl<Argument, Feedback, Result>::call_functor_type call_functor_type;

private:
  boost::shared_ptr<server_impl<Argument, Feedback, Result> > impl_;

public:
  server()
  {
  }

  server(boost::asio::io_service &io_service,
	 procedure_service &procedure_service) :
    impl_(boost::make_shared<server_impl<Argument, Feedback, Result> >(boost::ref(io_service),
								       boost::ref(procedure_service)))
  {
  }

  void call(const boost::shared_ptr<const Argument> &arg)
  {
    if(impl_.get() != 0)
    {
      impl_->publish(arg);
    }
  }

  void attach(const std::string& tag)
  {
    if(impl_.get() != 0)
    {
      impl_->attach(tag);
    }
  }

  void detach()
  {
    if(impl_.get() != 0)
    {
      impl_->detach();
    }
  }

  void set_call_handler(call_functor_type handler)
  {
    if(impl_.get() != 0)
    {
      impl_->set_call_handler(handler);
    }
  }

};

}
}
