/* Copyright (c) 2017-2023. The SimGrid Team. All rights reserved.               */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "src/3rd-party/catch.hpp"
#include "src/mc/explo/udpor/Unfolding.hpp"

using namespace simgrid::mc::udpor;

TEST_CASE("simgrid::mc::udpor::Unfolding: Creating an unfolding")
{
  Unfolding unfolding;
  REQUIRE(unfolding.size() == 0);
  REQUIRE(unfolding.empty());
}

TEST_CASE("simgrid::mc::udpor::Unfolding: Inserting and removing events with an unfolding")
{
  Unfolding unfolding;
  auto e1        = std::make_unique<UnfoldingEvent>();
  auto e2        = std::make_unique<UnfoldingEvent>();
  auto e1_handle = e1.get();
  auto e2_handle = e2.get();

  unfolding.insert(std::move(e1));
  REQUIRE(unfolding.size() == 1);
  REQUIRE_FALSE(unfolding.empty());

  unfolding.insert(std::move(e2));
  REQUIRE(unfolding.size() == 2);
  REQUIRE_FALSE(unfolding.empty());

  unfolding.remove(e1_handle);
  REQUIRE(unfolding.size() == 1);
  REQUIRE_FALSE(unfolding.empty());

  unfolding.remove(e2_handle);
  REQUIRE(unfolding.size() == 0);
  REQUIRE(unfolding.empty());
}