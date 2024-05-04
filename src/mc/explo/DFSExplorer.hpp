/* Copyright (c) 2008-2024. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#ifndef SIMGRID_MC_SAFETY_CHECKER_HPP
#define SIMGRID_MC_SAFETY_CHECKER_HPP

#include "src/mc/api/ClockVector.hpp"
#include "src/mc/api/states/State.hpp"
#include "src/mc/explo/Exploration.hpp"
#include "src/mc/explo/odpor/Execution.hpp"
#include "src/mc/explo/reduction/DPOR.hpp"
#include "src/mc/mc_config.hpp"

#include <deque>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace simgrid::mc {

using stack_t     = std::deque<std::shared_ptr<State>>;
using EventHandle = uint32_t;

class XBT_PRIVATE DFSExplorer : public Exploration {
private:
  std::unique_ptr<Reduction> reduction_algo_;
  ReductionMode reduction_mode_;
  unsigned long backtrack_count_      = 0; // for statistics
  unsigned long visited_states_count_ = 0; // for statistics

  static xbt::signal<void(RemoteApp&)> on_exploration_start_signal;
  static xbt::signal<void(RemoteApp&)> on_backtracking_signal;

  static xbt::signal<void(State*, RemoteApp&)> on_state_creation_signal;

  static xbt::signal<void(State*, RemoteApp&)> on_restore_system_state_signal;
  static xbt::signal<void(RemoteApp&)> on_restore_initial_state_signal;
  static xbt::signal<void(Transition*, RemoteApp&)> on_transition_replay_signal;
  static xbt::signal<void(Transition*, RemoteApp&)> on_transition_execute_signal;

  static xbt::signal<void(RemoteApp&)> on_log_state_signal;

  stack_t stack_;

public:
  explicit DFSExplorer(const std::vector<char*>& args, ReductionMode mode);
  void run() override;
  RecordTrace get_record_trace() override;
  void log_state() override;

  /** Called once when the exploration starts */
  static void on_exploration_start(std::function<void(RemoteApp& remote_app)> const& f)
  {
    on_exploration_start_signal.connect(f);
  }
  /** Called each time that the exploration backtracks from a exploration end */
  static void on_backtracking(std::function<void(RemoteApp& remote_app)> const& f)
  {
    on_backtracking_signal.connect(f);
  }
  /** Called each time that a new state is create */
  static void on_state_creation(std::function<void(State*, RemoteApp& remote_app)> const& f)
  {
    on_state_creation_signal.connect(f);
  }
  /** Called when rollbacking directly onto a previously checkpointed state */
  static void on_restore_system_state(std::function<void(State*, RemoteApp& remote_app)> const& f)
  {
    on_restore_system_state_signal.connect(f);
  }
  /** Called when the state to which we backtrack was not checkpointed state, forcing us to restore the initial state
   * before replaying some transitions */
  static void on_restore_initial_state(std::function<void(RemoteApp& remote_app)> const& f)
  {
    on_restore_initial_state_signal.connect(f);
  }
  /** Called when replaying a transition that was previously executed, to reach a backtracked state */
  static void on_transition_replay(std::function<void(Transition*, RemoteApp& remote_app)> const& f)
  {
    on_transition_replay_signal.connect(f);
  }
  /** Called when executing a new transition */
  static void on_transition_execute(std::function<void(Transition*, RemoteApp& remote_app)> const& f)
  {
    on_transition_execute_signal.connect(f);
  }
  /** Called when displaying the statistics at the end of the exploration */
  static void on_log_state(std::function<void(RemoteApp&)> const& f) { on_log_state_signal.connect(f); }

private:
  bool is_execution_descending = true;

  void explore(odpor::Execution S, stack_t state_stack);
  void simgrid_wrapper_explore(odpor::Execution S, aid_t next_actor, stack_t state_stack);
};

} // namespace simgrid::mc

#endif
