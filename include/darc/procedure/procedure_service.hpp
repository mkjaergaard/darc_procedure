#pragma once

#include <darc/procedure/procedure_service__decl.hpp>
#include <darc/procedure/local_dispatcher.hpp>

namespace darc
{
namespace procedure
{

template<typename Argument, typename Result, typename Feedback>
local_dispatcher<Argument, Result, Feedback>*
procedure_service::get_dispatcher(const tag_handle& tag)
{
  typename dispatcher_list_type::iterator elem = dispatcher_list_.find(tag->id());
  if(elem == dispatcher_list_.end())
  {
    boost::shared_ptr<local_dispatcher<Argument, Result, Feedback> > dispatcher
      = boost::make_shared<local_dispatcher<Argument, Result, Feedback> >(
	tag);

    dispatcher_list_.insert(
      typename dispatcher_list_type::value_type(tag->id(), dispatcher));

    return dispatcher.get();
  }
  else
  {
    boost::shared_ptr<local_dispatcher_base> &dispatcher_base = elem->second;
    // todo, try
    boost::shared_ptr<local_dispatcher<Argument, Result, Feedback> > dispatcher
      = boost::dynamic_pointer_cast<local_dispatcher<Argument, Result, Feedback> >(dispatcher_base);
    return dispatcher.get();
  }
}

}
}
