/* Copyright (c) 2017-2021. The SimGrid Team. All rights reserved.               */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "catch.hpp"

#include "simgrid/kernel/routing/FullZone.hpp"
#include "simgrid/kernel/routing/NetPoint.hpp"
#include "simgrid/s4u/Engine.hpp"
#include "simgrid/s4u/Host.hpp"
#include "simgrid/s4u/NetZone.hpp"
#include "src/surf/network_interface.hpp" //LinkImpl

namespace {
class EngineWrapper {
  int argc = 1;
  char* argv;
  simgrid::s4u::Engine e;

public:
  explicit EngineWrapper(std::string name) : argv(&name[0]), e(&argc, &argv) {}
};
} // namespace

TEST_CASE("kernel::routing::FullZone: Creating Zone", "")
{
  EngineWrapper e("test");

  REQUIRE(simgrid::s4u::create_full_zone("test"));
}

TEST_CASE("kernel::routing::FullZone: mix new routes and hosts", "[bug]")
{
  EngineWrapper e("test");
  auto* zone = simgrid::s4u::create_full_zone("test");

  const simgrid::s4u::Host* nic = zone->create_host("nic", 1e9)->seal();
  simgrid::s4u::Link* link      = zone->create_link("my_link", 1e6)->seal();
  for (int i = 0; i < 10; i++) {
    std::string cpu_name          = "CPU" + std::to_string(i);
    const simgrid::s4u::Host* cpu = zone->create_host(cpu_name, 1e9)->seal();
    REQUIRE_NOTHROW(zone->add_route(cpu->get_netpoint(), nic->get_netpoint(), nullptr, nullptr,
                                    std::vector<simgrid::s4u::Link*>{link}, true));
  }
}