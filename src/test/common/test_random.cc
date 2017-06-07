// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2017 SUSE LINUX GmbH
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
*/

#include <sstream>

#include "include/random.h"

#include "gtest/gtest.h"

// Helper to see if calls compile with various types:
template <typename T>
T type_check_ok(const T min, const T max)
{
 return ceph::util::generate_random_number(min, max);
}

/* This mixes integer and real types freely, so it's not the greatest
of tests, but should work casually for us: */
template <typename X0, typename X1>
bool none_equal(const X0 x0, const X1 x1)
{
 return x0 != x1;
}

template <typename X0, typename X1, typename ...XS>
bool none_equal(const X0 x0, const X1 x1, const XS ...xs)
{
 return none_equal(x0, none_equal(x1, xs...));
}

// Mini-examples showing canonical usage:
TEST(util, test_random_canonical)
{
 // Seed random number generation:
 ceph::util::randomize_rng();

 // Get a random int between 0 and max int:
 auto a = ceph::util::generate_random_number();

 // Get a random int between 0 and 20:
 auto b = ceph::util::generate_random_number(20);

 // Get a random int between 1 and 20:
 auto c = ceph::util::generate_random_number(1, 20);

 // Get a random float between 0.0 and 20.0:
 auto d = ceph::util::generate_random_number(20.0);

 // Get a random float between 0.001 and 0.991:
 auto e = ceph::util::generate_random_number(0.001, 0.991);

 // Make a random number function suitable for putting on its own thread:
 auto gen_f = ceph::util::make_random_number_function<1, 20>();
 auto x = gen_f();

 // Same, with seed (no re-seeding is possible):
 auto gen_g = ceph::util::make_random_number_function<1, 20, 42>();
 auto y = gen_g();

 // Make a function object RNG suitable for putting on its own thread:
 auto gen_fn = ceph::util::random_number_generator<int>();
 auto z = gen_fn();
 gen_fn.seed(42);   // re-seed

 // Mostly to swallow "unused variable" warnings:
 ASSERT_EQ(true, none_equal(a, b, c, d, e, x, y, z));
}

TEST(util, test_random)
{
 /* The intent of this test is not to formally test random number generation, but rather to
 casually check that "it works" and catch regressions: */

 // The default overload should compile:
 ceph::util::randomize_rng();

 {
 int a = ceph::util::generate_random_number();
 int b = ceph::util::generate_random_number();

 // Technically, this can still collide and cause a false negative, but let's be optimistic:
 if(std::numeric_limits<int>::max() > 32767)
  {
    ASSERT_GT(a, -1);
    ASSERT_GT(b, -1);

    ASSERT_NE(a, b);
  }
 }

 {
 auto a = ceph::util::generate_random_number(1, std::numeric_limits<int>::max());
 auto b = ceph::util::generate_random_number(1, std::numeric_limits<int>::max());

 if(std::numeric_limits<int>::max() > 32767)
  {
    ASSERT_GT(a, 0);
    ASSERT_GT(b, 0);

    ASSERT_NE(a, b);
  }
 }

 for(auto n = 100000; n; --n)
  {
    constexpr int min = 0, max = 6;
    int a = ceph::util::generate_random_number<min, max>();
    ASSERT_GT(a, -1);
    ASSERT_LT(a, 7);
  }

 // Multiple types (integral):
 {
 int min = 0, max = 1;
 type_check_ok(min, max);
 }

 {
 long min = 0, max = 1l;
 type_check_ok(min, max);
 }

 // Multiple types (floating point):
 {
 double min = 0.0, max = 1.0;
 type_check_ok(min, max);
 }

 {
 float min = 0.0, max = 1.0;
 type_check_ok(min, max);
 }

 // min > max should not explode:
 {
 float min = 1.0, max = 0.0;
 auto x = ceph::util::generate_random_number(min, max);
 }
}

TEST(util, test_user_mutex)
{
 // Show that we can call with a user lock:
 std::mutex l;

 ceph::util::randomize_rng(l);

 ceph::util::generate_random_number(l);

 ceph::util::generate_random_number(20, l);
 ceph::util::generate_random_number(1, 20, l);
 ceph::util::generate_random_number(20l, l);
 ceph::util::generate_random_number(1l, 20l, l);

 ceph::util::generate_random_number(20.0, l);
 ceph::util::generate_random_number(1.0, 20.0, l);
 ceph::util::generate_random_number(20.0d, l);
 ceph::util::generate_random_number(1.0d, 20.0d, l);
}

TEST(util, test_random_user_rng_lock)
{
 /* Users may not want to use the static RNG-- for example,
 in a case where the developer would like one RNG per thread. For
 now, we support a custom RNG only along with an accompanying
 custom lock through the function interface: */

 std::mutex l;
 std::mt19937_64 e;

 ceph::util::randomize_rng(l, e);

 ceph::util::generate_random_number(l, e);

 ceph::util::generate_random_number(20, l, e);
 ceph::util::generate_random_number(1, 20, l, e);

 ceph::util::generate_random_number(20l, l, e);

 ceph::util::generate_random_number(1l, 20l, l, e);

 ceph::util::generate_random_number(20.0, l, e);
 ceph::util::generate_random_number(1.0, 20.0, l, e);

 ceph::util::generate_random_number(20.0d, l, e);
 ceph::util::generate_random_number(1.0d, 20.0d, l, e);
}

TEST(util, test_make_random_function)
{
 // Test convenience functions:

 constexpr auto max_int   = std::numeric_limits<int>::max();

 {
 auto rng_i = ceph::util::make_random_number_function<1, max_int - 1>();

 int x = rng_i(), y = rng_i();

 ASSERT_GT(x, 0);
 ASSERT_LT(x, max_int);
 ASSERT_GT(y, 0);
 ASSERT_LT(y, max_int);
 ASSERT_NE(x, y);
 }
 
 // User may specify engine:
 {
 auto rng_i = ceph::util::make_random_number_function<1, 10, std::mt19937_64>();
 int x = rng_i();
 ASSERT_GT(x, 0); 
 ASSERT_LT(x, 10); 
 }

 // User specified seed:
 {
 auto rng_i = ceph::util::make_random_number_function<1, 10, 42>();
 int x = rng_i();
 ASSERT_GT(x, 0); 
 ASSERT_LT(x, 10); 
 }

}

TEST(util, test_random_class_interface)
{
 ceph::util::random_number_generator<int> rng;

 // Other ctors:
 {
 ceph::util::random_number_generator<int> rng(1234);   // seed
 }

 {
 auto a = rng();
 auto b = rng();

 // Technically can fail, but should "almost never" happen:
 ASSERT_NE(a, b);
 }

 {
 auto a = rng(10);
 ASSERT_LT(a, 11);
 ASSERT_GT(a, -1);
 }

 {
 auto a = rng(10.0);
 ASSERT_LT(a, 11.0);
 ASSERT_GT(a, -1.0);
 }

 {
 auto a = rng(10, 20);
 ASSERT_LT(a, 21);
 ASSERT_GT(a, 9);
 }

 {
 auto a = rng(10.0, 20.0);
 ASSERT_LT(a, 20.1);
 ASSERT_GT(a, 9.9);
 }
}

