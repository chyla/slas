/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include <gmock/gmock.h>

#include "tests/mock/apache/database/database_functions.h"

using namespace testing;
using namespace std;

class apache_DatabaseFunctionsTest : public ::testing::Test {
 public:
  virtual ~apache_DatabaseFunctionsTest() = default;

  void SetUp() {
    database_functions = ::mock::apache::database::DatabaseFunctions::Create();
  }

  void TearDown() {
  }

  ::mock::apache::database::DatabaseFunctionsPtr database_functions;
};

TEST_F(apache_DatabaseFunctionsTest, Create) {
}
