/* Copyright (c) 2007-2024. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#ifndef SIMGRID_MC_SDPOR_HPP
#define SIMGRID_MC_SDPOR_HPP

#include "simgrid/forward.h"
#include "src/mc/explo/odpor/Execution.hpp"
#include "src/mc/explo/reduction/Reduction.hpp"

#include "src/mc/api/states/SleepSetState.hpp"

namespace simgrid::mc {

class SDPOR : public Reduction {

public:
  SDPOR()           = default;
  ~SDPOR() override = default;

  void races_computation(odpor::Execution E, stack_t* S, std::vector<std::shared_ptr<State>>* opened_states) override
  {
    // If there are less then 2 events, there is no possible race yet
    if (E.size() <= 1)
      return;

    const auto next_E_p = E.get_latest_event_handle().value();
    for (const auto e_race : E.get_reversible_races_of(next_E_p)) {

      State* prev_state  = (*S)[e_race].get();
      const auto choices = E.get_missing_source_set_actors_from(e_race, prev_state->get_backtrack_set());
      if (not choices.empty()) {
        prev_state->ensure_one_considered_among_set(choices);
        if (opened_states != nullptr)
          opened_states->emplace_back(prev_state);
      }
    }
  }

  std::shared_ptr<State> state_create(RemoteApp& remote_app, std::shared_ptr<State> parent_state) override
  {

    std::shared_ptr<SleepSetState> sleep_set_state =
        std::static_pointer_cast<SleepSetState>(Reduction::state_create(remote_app, parent_state));

    if (not sleep_set_state->get_enabled_minus_sleep().empty()) {
      sleep_set_state->consider_best();
    }

    return sleep_set_state;
  }

  aid_t next_to_explore(odpor::Execution E, stack_t* S) override
  {
    if (S->back()->get_batrack_minus_done().empty())
      return -1;
    return S->back()->next_transition_guided().first;
  }
};

} // namespace simgrid::mc

#endif
