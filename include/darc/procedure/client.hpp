#pragma once

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/signals.hpp>

#include <darc/procedure/procedure_service__decl.hpp>
#include <darc/procedure/local_dispatcher__fwd.hpp>

#include <darc/id.hpp>

#include <boost/make_shared.hpp>

namespace darc
{
namespace procedure
{

template<typename Argument, typename Feedback, typename Result>
class client_impl
{
private:
  boost::asio::io_service &io_service_;
  procedure_service &procedure_service_;

  local_dispatcher<Argument, Feedback, Result> * dispatcher_;

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
      return dispatcher_->call(this, argument);
    }
    assert(false);
  }

};

template<typename Argument, typename Feedback, typename Result>
class client
{
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

};

}
}
