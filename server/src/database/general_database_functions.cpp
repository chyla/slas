/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "general_database_functions.h"

#include <iostream>
#include <boost/log/trivial.hpp>

#include "exception/detail/cant_execute_sql_statement_exception.h"
#include "exception/detail/item_not_found_exception.h"
#include "exception/detail/item_not_found_exception.h"
#include "database.h"

using namespace std;

namespace database
{

GeneralDatabaseFunctionsPtr GeneralDatabaseFunctions::Create(::database::DatabasePtr database, detail::SQLiteWrapperInterfacePtr sqlite_wrapper) {
  return GeneralDatabaseFunctionsPtr(new GeneralDatabaseFunctions(database, sqlite_wrapper));
}

void GeneralDatabaseFunctions::CreateTables() {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::CreateTables: Function call";

  sqlite_wrapper_->Exec("create table if not exists TIME_TABLE ("
                        "  ID integer primary key, "
                        "  HOUR integer, "
                        "  MINUTE integer, "
                        "  SECOND integer, "
                        "  unique(HOUR, MINUTE, SECOND) "
                        ");");

  sqlite_wrapper_->Exec("create table if not exists DATE_TABLE ("
                        "  ID integer primary key, "
                        "  DAY integer, "
                        "  MONTH integer, "
                        "  YEAR integer, "
                        "  unique(DAY, MONTH, YEAR) "
                        ");");

  sqlite_wrapper_->Exec("create table if not exists AGENT_NAMES ( "
                        "  ID integer primary key not null, "
                        "  AGENT_NAME text, "
                        "  unique (AGENT_NAME) "
                        ");");
}

void GeneralDatabaseFunctions::AddTime(const ::type::Time &t) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::AddTime: Function call";

  sqlite_wrapper_->Exec("insert or replace into TIME_TABLE (ID, HOUR, MINUTE, SECOND) "
                        "values ( "
                        " (select ID from TIME_TABLE where "
                        "    HOUR=" + to_string(t.GetHour()) + " and "
                        "    MINUTE=" + to_string(t.GetMinute()) + " and "
                        "    SECOND=" + to_string(t.GetSecond()) + "),"
                        + to_string(t.GetHour()) + ", "
                        + to_string(t.GetMinute()) + ", "
                        + to_string(t.GetSecond())
                        + " );");
}

::database::type::RowId GeneralDatabaseFunctions::AddAndGetTimeId(const ::type::Time &time) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::AddAndGetTimeId: Function call";

  auto time_id = GetTimeId(time);
  if (time_id == -1) {
    AddTime(time);
    time_id = GetTimeId(time);
  }

  return time_id;
}

::database::type::RowId GeneralDatabaseFunctions::GetTimeId(const ::type::Time &t) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::GetDateId: Function call";

  const string sql =
      "select id from TIME_TABLE "
      "  where"
      "    HOUR=" + to_string(t.GetHour()) + " and "
      "    MINUTE=" + to_string(t.GetMinute()) + " and "
      "    SECOND=" + to_string(t.GetSecond()) +
      ";";

  return sqlite_wrapper_->GetFirstInt64Column(sql, -1);
}

const ::type::Time GeneralDatabaseFunctions::GetTimeById(::database::type::RowId id) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::GetTimeById: Function call";
  ::type::Time t;
  bool found = false;

  string sql =
      "select HOUR, MINUTE, SECOND from TIME_TABLE "
      "  where"
      "    ID=" + to_string(id) + ";";

  sqlite3_stmt *statement = nullptr;
  sqlite_wrapper_->Prepare(sql, &statement);

  auto ret = sqlite_wrapper_->Step(statement);
  if (ret == SQLITE_ROW) {
    t.Set(sqlite_wrapper_->ColumnInt(statement, 0),
          sqlite_wrapper_->ColumnInt(statement, 1),
          sqlite_wrapper_->ColumnInt(statement, 2));
    found = true;
  }

  sqlite_wrapper_->Finalize(statement);

  if (!found) {
    BOOST_LOG_TRIVIAL(error) << "database::GeneralDatabaseFunctions::GetTimeById: Time not found";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  return t;
}

void GeneralDatabaseFunctions::AddDate(const ::type::Date &date) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::AddDate: Function call";

  sqlite_wrapper_->Exec(" insert or ignore into DATE_TABLE (DAY, MONTH, YEAR) "
                        " values ( "
                        + to_string(date.GetDay()) + ", " + to_string(date.GetMonth()) + ", " + to_string(date.GetYear())
                        + " );");
}

::database::type::RowId GeneralDatabaseFunctions::AddAndGetDateId(const ::type::Date &date) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::AddAndGetDateId: Function call";

  auto date_id = GetDateId(date);
  if (date_id == -1) {
    AddDate(date);
    date_id = GetDateId(date);
  }

  return date_id;
}

::database::type::RowId GeneralDatabaseFunctions::GetDateId(const ::type::Date &date) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::GetDateId: Function call";

  string sql =
      "select id from DATE_TABLE "
      "  where"
      "    DAY=" + to_string(date.GetDay()) + " and "
      "    MONTH=" + to_string(date.GetMonth()) + " and "
      "    YEAR=" + to_string(date.GetYear()) +
      ";";

  return sqlite_wrapper_->GetFirstInt64Column(sql, -1);
}

::database::type::RowIds GeneralDatabaseFunctions::GetDateRangeIds(const ::type::Date &from, const ::type::Date &to) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::GetDateRangeIds: Function call";

  string from_day = to_string(from.GetDay()),
      from_month = to_string(from.GetMonth()),
      from_year = to_string(from.GetYear()),
      to_day = to_string(to.GetDay()),
      to_month = to_string(to.GetMonth()),
      to_year = to_string(to.GetYear());

  string sql =
      "select ID from DATE_TABLE where "
      "  ("
      "    ("
      "      (YEAR > " + from_year + ")"
      "      or (YEAR = " + from_year + " and MONTH > " + from_month + ")"
      "      or (YEAR = " + from_year + " and MONTH = " + from_month + " and DAY >= " + from_day + ")"
      "    )"
      "  and"
      "    ("
      "      (YEAR < " + to_year + ")"
      "      or (YEAR = " + to_year + " and MONTH < " + to_month + ")"
      "      or (YEAR = " + to_year + " and MONTH = " + to_month + " and DAY <= " + to_day + ")"
      "    )"
      "  )"
      ";"
      ;

  ::database::type::RowIds ids;
  ::database::type::RowId id;

  sqlite3_stmt *statement = nullptr;
  sqlite_wrapper_->Prepare(sql, &statement);

  try {
    do {
      auto ret = sqlite_wrapper_->Step(statement);

      if (ret == SQLITE_ROW) {
        id = sqlite_wrapper_->ColumnInt64(statement, 0);
        ids.push_back(id);
      }
      else
        break;
    }
    while (true);
  }
  catch (::database::exception::DatabaseException &ex) {
    BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::GetDateRangeIds: Exception catched: " << ex.what();
    sqlite_wrapper_->Finalize(statement);
    throw;
  }

  sqlite_wrapper_->Finalize(statement);

  return ids;
}

::type::Date GeneralDatabaseFunctions::GetDateById(const ::database::type::RowId &id) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::GetDateById: Function call";
  ::type::Date date;
  bool found = false;

  string sql =
      "select DAY, MONTH, YEAR from DATE_TABLE "
      "  where"
      "    ID=" + to_string(id) + ";";

  sqlite3_stmt *statement = nullptr;
  sqlite_wrapper_->Prepare(sql, &statement);

  auto ret = sqlite_wrapper_->Step(statement);
  if (ret == SQLITE_ROW) {
    date.Set(sqlite_wrapper_->ColumnInt(statement, 0),
             sqlite_wrapper_->ColumnInt(statement, 1),
             sqlite_wrapper_->ColumnInt(statement, 2));
    found = true;
  }

  sqlite_wrapper_->Finalize(statement);

  if (!found) {
    BOOST_LOG_TRIVIAL(error) << "database::GeneralDatabaseFunctions::GetTimeById: Date with id=" << id << " not found";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  return date;
}

void GeneralDatabaseFunctions::AddAgentName(const std::string &name) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::AddAgentName: Function call";

  string sql = "insert or ignore into AGENT_NAMES ( AGENT_NAME ) values (?);";

  sqlite3_stmt *statement = nullptr;
  sqlite_wrapper_->Prepare(sql, &statement);

  try {
    sqlite_wrapper_->BindText(statement, 1, name);
    sqlite_wrapper_->Step(statement);
  }
  catch (exception::DatabaseException &ex) {
    sqlite_wrapper_->Finalize(statement);
    throw;
  }

  sqlite_wrapper_->Finalize(statement);
}

type::AgentNames GeneralDatabaseFunctions::GetAgentNames() {
  return database_->GetApacheAgentNames();
}

::database::type::RowIds GeneralDatabaseFunctions::GetAgentsIds() {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::GetAgentsIds: Function call";

  ::database::type::RowIds ids;
  ::database::type::RowId id;

  const char *sql = "select ID from AGENT_NAMES;";

  sqlite3_stmt *statement = nullptr;
  sqlite_wrapper_->Prepare(sql, &statement);

  try {
    do {
      auto ret = sqlite_wrapper_->Step(statement);

      if (ret == SQLITE_ROW) {
        id = sqlite_wrapper_->ColumnInt64(statement, 0);
        ids.push_back(id);
      }
      else
        break;
    }
    while (true);
  }
  catch (::database::exception::DatabaseException &ex) {
    BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::GetAgentsIds: Exception catched: " << ex.what();
    sqlite_wrapper_->Finalize(statement);
    throw;
  }

  sqlite_wrapper_->Finalize(statement);

  return ids;
}

::database::type::RowId GeneralDatabaseFunctions::AddAndGetAgentNameId(const std::string &name) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::AddAndGetAgentNameId: Function call";

  AddAgentName(name);
  auto id = GetAgentNameId(name);

  return id;
}

::database::type::RowId GeneralDatabaseFunctions::GetAgentNameId(const std::string &name) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::GetAgentNameId: Function call";
  ::database::type::RowId id = -1;

  string sql = "select ID from AGENT_NAMES where AGENT_NAME=?";

  sqlite3_stmt *statement = nullptr;
  sqlite_wrapper_->Prepare(sql, &statement);

  try {
    sqlite_wrapper_->BindText(statement, 1, name);

    auto ret = sqlite_wrapper_->Step(statement);
    if (ret == SQLITE_ROW)
      id = sqlite_wrapper_->ColumnInt64(statement, 0);
  }
  catch (exception::DatabaseException &ex) {
    sqlite_wrapper_->Finalize(statement);
    throw;
  }

  sqlite_wrapper_->Finalize(statement);

  if (id < 0)
    throw ::database::exception::detail::ItemNotFoundException();

  return id;
}

std::string GeneralDatabaseFunctions::GetAgentNameById(const ::database::type::RowId &id) {
  BOOST_LOG_TRIVIAL(debug) << "database::GeneralDatabaseFunctions::GetAgentNameById: Function call";
  std::string name;
  bool found = false;

  string sql =
      "select AGENT_NAME from AGENT_NAMES "
      "  where"
      "    ID=" + to_string(id) + ";";

  sqlite3_stmt *statement = nullptr;
  sqlite_wrapper_->Prepare(sql, &statement);

  try {
    auto ret = sqlite_wrapper_->Step(statement);
    if (ret == SQLITE_ROW) {
      name = sqlite_wrapper_->ColumnText(statement, 0);
      found = true;
    }
  }
  catch (exception::DatabaseException &ex) {
    sqlite_wrapper_->Finalize(statement);
    throw;
  }

  sqlite_wrapper_->Finalize(statement);

  if (!found) {
    BOOST_LOG_TRIVIAL(error) << "database::GeneralDatabaseFunctions::GetAgentNameById: Item with id=" << id << " not found";
    throw exception::detail::ItemNotFoundException();
  }

  return name;
}

GeneralDatabaseFunctions::GeneralDatabaseFunctions(::database::DatabasePtr database,
                                                   detail::SQLiteWrapperInterfacePtr sqlite_wrapper) :
database_(database),
sqlite_wrapper_(sqlite_wrapper) {
}

}
