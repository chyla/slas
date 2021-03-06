/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#pragma once

#include "src/database/detail/general_database_functions_interface.h"

#include <memory>
#include <gmock/gmock.h>

namespace mock
{

namespace database
{

class GeneralDatabaseFunctions;
typedef std::shared_ptr<GeneralDatabaseFunctions> GeneralDatabaseFunctionsPtr;

class GeneralDatabaseFunctions : public ::database::detail::GeneralDatabaseFunctionsInterface {
 public:
  virtual ~GeneralDatabaseFunctions() = default;

  static GeneralDatabaseFunctionsPtr Create() {
    return std::make_shared<GeneralDatabaseFunctions>();
  }

  MOCK_METHOD0(CreateTables, void());

  MOCK_METHOD1(AddTime, void(const ::type::Time &t));
  MOCK_METHOD1(AddAndGetTimeId, ::database::type::RowId(const ::type::Time &time));
  MOCK_METHOD1(GetTimeId, ::database::type::RowId(const ::type::Time &t));
  MOCK_METHOD1(GetTimeById, const ::type::Time(::database::type::RowId id));

  MOCK_METHOD1(AddDate, void(const ::type::Date &date));
  MOCK_METHOD1(AddAndGetDateId, ::database::type::RowId(const ::type::Date &date));
  MOCK_METHOD2(GetDateRangeIds, ::database::type::RowIds(const ::type::Date &from, const ::type::Date &to));
  MOCK_METHOD1(GetDateId, ::database::type::RowId(const ::type::Date &date));
  MOCK_METHOD1(GetDateById, ::type::Date(const ::database::type::RowId &id));

  MOCK_METHOD1(AddAgentName, void (const std::string &name));
  MOCK_METHOD0(GetAgentNames, ::database::type::AgentNames());
  MOCK_METHOD0(GetAgentsIds, ::database::type::RowIds());
  MOCK_METHOD1(AddAndGetAgentNameId, ::database::type::RowId(const std::string &name));
  MOCK_METHOD1(GetAgentNameId, ::database::type::RowId(const std::string &name));
  MOCK_METHOD1(GetAgentNameById, std::string(const ::database::type::RowId &id));
};

}

}
