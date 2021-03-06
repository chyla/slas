/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <gmock/gmock.h>
#include <iostream>

#include "src/apache/analyzer/detail/prepare_statistics/nearest_neighbours_table.h"

using namespace testing;
using namespace std;
using namespace ::apache::analyzer::detail::prepare_statistics;
using namespace ::apache::type;

class NearestNeighboursTableTest : public ::testing::Test {
 public:
  virtual ~NearestNeighboursTableTest() = default;

  NearestNeighboursTable nearest_neighbours;
  ApacheSessionEntry session;
  ApacheSessionEntry s1, s2, s3, s4, s5;
  Neighbour n1, n2, n3, n4, n5;

  void SetUp() override {
    nearest_neighbours = NearestNeighboursTable();

    session.id = 1;
    session.bandwidth_usage = 50;
    session.error_percentage = 50;
    session.requests_count = 50;
    session.session_length = 50;

    s1.id = 2;
    s1.classification = ::database::type::Classification::NORMAL;
    s1.bandwidth_usage = 150;
    s1.error_percentage = 150;
    s1.requests_count = 150;
    s1.session_length = 150;

    n1.classification = ::database::type::Classification::NORMAL;
    n1.distance = 200;
    n1.session_id = 2;

    s2.id = 3;
    s2.classification = ::database::type::Classification::ANOMALY;
    s2.bandwidth_usage = 200;
    s2.error_percentage = 200;
    s2.requests_count = 200;
    s2.session_length = 200;

    n2.classification = ::database::type::Classification::ANOMALY;
    n2.distance = 300;
    n2.session_id = 3;

    s3.id = 4;
    s3.classification = ::database::type::Classification::NORMAL;
    s3.bandwidth_usage = 250;
    s3.error_percentage = 250;
    s3.requests_count = 250;
    s3.session_length = 250;

    n3.classification = ::database::type::Classification::NORMAL;
    n3.distance = 400;
    n3.session_id = 4;

    s4.id = 5;
    s4.classification = ::database::type::Classification::ANOMALY;
    s4.bandwidth_usage = 300;
    s4.error_percentage = 300;
    s4.requests_count = 300;
    s4.session_length = 300;

    n4.classification = ::database::type::Classification::ANOMALY;
    n4.distance = 500;
    n4.session_id = 5;

    s5.id = 6;
    s5.classification = ::database::type::Classification::UNKNOWN;
    s5.bandwidth_usage = 200;
    s5.error_percentage = 200;
    s5.requests_count = 200;
    s5.session_length = 200;

    n5.classification = ::database::type::Classification::UNKNOWN;
    n5.distance = 300;
    n5.session_id = 6;
  }

  void TearDown() override {
  }
};

TEST_F(NearestNeighboursTableTest, OneElement) {
  nearest_neighbours.SetSession(session);

  nearest_neighbours.Add(s1);

  auto &neighbours = nearest_neighbours.Get();

  EXPECT_EQ(1, neighbours.size());

  EXPECT_EQ(n1, neighbours.at(0));
}

TEST_F(NearestNeighboursTableTest, TwoElements) {
  nearest_neighbours.SetSession(session);

  nearest_neighbours.Add(s1);
  nearest_neighbours.Add(s2);

  auto &neighbours = nearest_neighbours.Get();

  EXPECT_EQ(2, neighbours.size());

  EXPECT_EQ(n1, neighbours.at(0));
  EXPECT_EQ(n2, neighbours.at(1));
}

TEST_F(NearestNeighboursTableTest, ThreeElements) {
  nearest_neighbours.SetSession(session);

  nearest_neighbours.Add(s1);
  nearest_neighbours.Add(s2);
  nearest_neighbours.Add(s3);

  auto &neighbours = nearest_neighbours.Get();

  EXPECT_EQ(3, neighbours.size());

  EXPECT_EQ(n1, neighbours.at(0));
  EXPECT_EQ(n2, neighbours.at(1));
  EXPECT_EQ(n3, neighbours.at(2));
}

TEST_F(NearestNeighboursTableTest, FourElements) {
  nearest_neighbours.SetSession(session);

  nearest_neighbours.Add(s1);
  nearest_neighbours.Add(s2);
  nearest_neighbours.Add(s3);
  nearest_neighbours.Add(s4);

  auto &neighbours = nearest_neighbours.Get();

  EXPECT_EQ(3, neighbours.size());

  EXPECT_EQ(n1, neighbours.at(0));
  EXPECT_EQ(n2, neighbours.at(1));
  EXPECT_EQ(n3, neighbours.at(2));
}

TEST_F(NearestNeighboursTableTest, MaxThenMiddle) {
  nearest_neighbours.SetSession(session);

  nearest_neighbours.Add(s3);
  nearest_neighbours.Add(s1);
  nearest_neighbours.Add(s4);
  nearest_neighbours.Add(s2);

  auto &neighbours = nearest_neighbours.Get();

  EXPECT_EQ(3, neighbours.size());

  EXPECT_EQ(n1, neighbours.at(0));
  EXPECT_EQ(n2, neighbours.at(1));
  EXPECT_EQ(n3, neighbours.at(2));
}

TEST_F(NearestNeighboursTableTest, ChangeSession) {
  nearest_neighbours.SetSession(session);

  nearest_neighbours.Add(s3);
  nearest_neighbours.Add(s1);
  nearest_neighbours.Add(s4);
  nearest_neighbours.Add(s2);

  nearest_neighbours.SetSession(session);

  nearest_neighbours.Add(s1);

  auto &neighbours = nearest_neighbours.Get();

  EXPECT_EQ(1, neighbours.size());
}

TEST_F(NearestNeighboursTableTest, AddThisSameElement) {
  nearest_neighbours.SetSession(session);

  nearest_neighbours.Add(s1);
  nearest_neighbours.Add(s1);

  auto &neighbours = nearest_neighbours.Get();

  EXPECT_EQ(1, neighbours.size());

  EXPECT_EQ(n1, neighbours.at(0));
}

TEST_F(NearestNeighboursTableTest, AddElementWithUnknownClassification) {
  nearest_neighbours.SetSession(session);

  nearest_neighbours.Add(s1);
  nearest_neighbours.Add(s4);
  nearest_neighbours.Add(s5);

  auto &neighbours = nearest_neighbours.Get();

  EXPECT_EQ(3, neighbours.size());

  EXPECT_EQ(n1, neighbours.at(0));
  EXPECT_EQ(n5, neighbours.at(1));
  EXPECT_EQ(n4, neighbours.at(2));
}
