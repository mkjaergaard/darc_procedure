#pragma once

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/signals.hpp>

#include <darc/procedure/procedure_service__decl.hpp>

#include <darc/id.hpp>

#include <boost/make_shared.hpp>

namespace darc
{
namespace procedure
{

// Fwd
template<typename, typename, typename>
class local_dispatcher;

// Class
template<typename Argument, typename Feedback, typename Result>
class client_impl
{
public:
  typedef void(feedback_handler_type)(const ID&, const boost::shared_ptr<const Feedback>&);
  typedef void(result_handler_type)(const ID&, const boost::shared_ptr<const Result>&);
  typedef boost::function<feedback_handler_type> feedback_functor_type;
  typedef boost::function<result_handler_type> result_functor_type;

private:
  boost::asio::io_service &io_service_;
  procedure_service &procedure_service_;

  local_dispatcher<Argument, Feedback, Result> * dispatcher_;
  feedback_functor_type feedback_handler_;
  result_functor_type result_handler_;

public:
  client_impl(boost::asio::io_service &io_service,
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

  // return a call handle
  ID call(const boost::shared_ptr<const Argument> &argument)
  {
    if(dispatcher_ != 0)
    {
      return dispatcher_->call_from_local(this, argument);
    }
    assert(false);
  }

  void set_feedback_handler(feedback_functor_type handler)
  {
    feedback_handler_ = handler;
  }

  void post_feedback(const ID& call_id, const boost::shared_ptr<const Feedback> &arg)
  {
    io_service_.post(boost::bind(&client_impl::trigger_feedback, this, call_id, arg));
  }

  void trigger_feedback(const ID& call_id, const boost::shared_ptr<const Feedback> &arg)
  {
    feedback_handler_(call_id, arg);
  }

  void set_result_handler(result_functor_type handler)
  {
    result_handler_ = handler;
  }

  void post_result(const ID& call_id, const boost::shared_ptr<const Argument> &arg)
  {
    io_service_.post(boost::bind(&client_impl::trigger_result, this, call_id, arg));
  }

  void trigger_result(const ID& call_id, const boost::shared_ptr<const Argument> &arg)
  {
    result_handler_(call_id, arg);
  }

};

template<typename Argument, typename Feedback, typename Result>
class client
{
public:
  typedef typename client_impl<Argument, Feedback, Result>::feedback_functor_type feedback_functor_type;
  typedef typename client_impl<Argument, Feedback, Result>::result_functor_type result_functor_type;

private:
  boost::shared_ptr<client_impl<Argument, Feedback, Result> > impl_;

public:
  client()
  {
  }

  client(boost::asio::io_service &io_service,
	 procedure_service &procedure_service) :
    impl_(boost::make_shared<client_impl<Argument, Feedback, Result> >(boost::ref(io_service),
								       boost::ref(procedure_service)))
  {
  }

  ID call(const boost::shared_ptr<const Argument> &arg)
  {
    if(impl_.get() != 0)
    {
      return impl_->call(arg);
    }
    assert(false);
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

  void set_feedback_handler(feedback_functor_type handler)
  {
    if(impl_.get() != 0)
    {
      impl_->set_feedback_handler(handler);
    }
  }

  void set_result_handler(result_functor_type handler)
  {
    if(impl_.get() != 0)
    {
      impl_->set_result_handler(handler);
    }
  }

};

}
}
