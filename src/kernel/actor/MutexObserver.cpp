/* Copyright (c) 2019-2022. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "src/kernel/actor/MutexObserver.hpp"
#include "simgrid/s4u/Host.hpp"
#include "src/kernel/activity/MutexImpl.hpp"
#include "src/kernel/actor/ActorImpl.hpp"
#include "src/mc/mc_config.hpp"

#include <sstream>

XBT_LOG_NEW_DEFAULT_SUBCATEGORY(obs_mutex, mc_observer, "Logging specific to mutex simcalls observation");

namespace simgrid {
namespace kernel {
namespace actor {

#if 0
bool MutexSimcall::depends(SimcallObserver* other)
{
  if (dynamic_cast<RandomSimcall*>(other) != nullptr)
    return other->depends(this); /* Other is random, that is very permissive. Use that relation instead. */

#if 0 /* This code is currently broken and shouldn't be used. We must implement asynchronous locks before */
  MutexSimcall* that = dynamic_cast<MutexSimcall*>(other);
  if (that == nullptr)
    return true; // Depends on anything we don't know

  /* Theorem 4.4.7: Any pair of synchronization actions of distinct actors concerning distinct mutexes are independent */
  if (this->get_issuer() != that->get_issuer() && this->get_mutex() != that->get_mutex())
    return false;

  /* Theorem 4.4.8 An AsyncMutexLock is independent with a MutexUnlock of another actor */
  if (((dynamic_cast<MutexLockSimcall*>(this) != nullptr && dynamic_cast<MutexUnlockSimcall*>(that)) ||
       (dynamic_cast<MutexLockSimcall*>(that) != nullptr && dynamic_cast<MutexUnlockSimcall*>(this))) &&
      get_issuer() != other->get_issuer())
    return false;
#endif
  return true; // Depend on things we don't know for sure that they are independent
}
#endif

MutexObserver::MutexObserver(ActorImpl* actor, activity::MutexImpl* mutex) : SimcallObserver(actor), mutex_(mutex) {}
MutexTestObserver::MutexTestObserver(ActorImpl* actor, activity::MutexImpl* mutex) : MutexObserver(actor, mutex) {}

MutexLockAsyncObserver::MutexLockAsyncObserver(ActorImpl* actor, activity::MutexImpl* mutex)
    : MutexObserver(actor, mutex)
{
}
MutexLockWaitObserver::MutexLockWaitObserver(ActorImpl* actor, activity::MutexAcquisitionImplPtr synchro)
    : MutexObserver(actor, synchro->get_mutex().get()), synchro_(synchro)
{
}

bool MutexLockWaitObserver::is_enabled()
{
  return synchro_->test();
}

} // namespace actor
} // namespace kernel
} // namespace simgrid