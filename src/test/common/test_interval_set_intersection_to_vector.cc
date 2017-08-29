// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2015 Mirantis, Inc.
 *
 * Author: Igor Fedotov <ifedotov@mirantis.com>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include <gtest/gtest.h>
#include <vector>
#include "include/interval_set.h"

using namespace ceph;

typedef uint64_t IntervalValueType;

template<typename T>  // tuple<type to test on, test array size>
class IntervalSetTest : public ::testing::Test {

 public:
  typedef T ISet;
};

typedef ::testing::Types< interval_set<IntervalValueType> > IntervalSetTypes;

TYPED_TEST_CASE(IntervalSetTest, IntervalSetTypes);

TYPED_TEST(IntervalSetTest, compare) {
  typedef typename TestFixture::ISet ISet;
  ISet iset1;
  std::vector<std::pair<const IntervalValueType,IntervalValueType>> v1;
  ASSERT_TRUE(iset1.equals(v1));

  iset1.insert(1);
  ASSERT_FALSE(iset1.equals(v1));

  v1.push_back(std::make_pair<IntervalValueType,IntervalValueType>(1, 1));
  ASSERT_TRUE(iset1.equals(v1));

  v1.push_back(std::make_pair<IntervalValueType,IntervalValueType>(3, 1));
  ASSERT_FALSE(iset1.equals(v1));
}

TYPED_TEST(IntervalSetTest, intersect_of) {
  typedef typename TestFixture::ISet ISet;
  ISet iset1, iset2, iset3;
  std::vector<std::pair<const IntervalValueType,IntervalValueType>> v1, v2;

  ASSERT_TRUE( iset1.num_intervals() == 0);
  ASSERT_TRUE( iset1.size() == 0);
  ASSERT_EQ( v1.size(), (IntervalValueType)0);

  iset2.insert( 0, 1 );
  iset2.insert( 5, 10 );
  iset2.insert( 30, 10 );

  iset1 = iset2;
  iset3 = iset2;

  iset1.insert(40, 5);

  auto consumer = [&](const std::pair<const IntervalValueType,IntervalValueType> &i){ v2.push_back(i); };
  iset3.copy(consumer);
  iset1.intersection_to_vector(v2, v1);
  ASSERT_EQ( v1.size(), (IntervalValueType)3);
  ASSERT_TRUE(iset2.equals(v1));
}

TYPED_TEST(IntervalSetTest, subtract) {
  typedef typename TestFixture::ISet ISet;
  ISet iset1, iset2;
  std::vector<std::pair<const IntervalValueType,IntervalValueType>> v1, v2;

  iset2.insert(5,5);
  iset2.insert(20,5);

  iset1 = iset2;
  iset2.insert(40,5);
  auto consumer = [&](const std::pair<const IntervalValueType,IntervalValueType> &i){ v2.push_back(i); };
  iset2.copy(consumer);

  iset1.intersection_to_vector(v2, v1);
  ASSERT_TRUE( iset1.equals(v1) );

  iset2.subtract(v1);
  ASSERT_EQ( iset2.size(), (IntervalValueType)5);
  ASSERT_EQ( iset2.num_intervals(),  1);
}
