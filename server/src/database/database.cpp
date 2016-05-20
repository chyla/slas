#include "database.h"

#include <utility>
#include <boost/log/trivial.hpp>
#include <patlms/type/bash_log_entry.h>
#include <patlms/type/apache_log_entry.h>

#include "detail/sqlite.h"
#include "src/database/exception/detail/cant_open_database_exception.h"
#include "src/database/exception/detail/cant_close_database_exception.h"
#include "src/database/exception/detail/cant_execute_sql_statement_exception.h"


namespace database
{

DatabasePtr Database::Create() {
  std::unique_ptr<detail::SQLite> sqlite(new detail::SQLite());
  return Database::Create(std::move(sqlite));
}

DatabasePtr Database::Create(std::unique_ptr<detail::SQLiteInterface> sqlite) {
  DatabasePtr db(new Database(move(sqlite)));
  return db;
}

void Database::Open(const std::string &file_path) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::Open: Function call";

  int ret = sqlite_interface_->Open(file_path.c_str(), &db_handle_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
  if (ret != SQLITE_OK) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::Open: Open error: " << ret;
    throw exception::detail::CantOpenDatabaseException();
  }

  is_open_ = true;
}

bool Database::IsOpen() const {
  return is_open_;
}

void Database::CreateBashLogsTable() {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::CreateBashLogsTable: Function call";

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::CreateBashLogsTable: Database is not open.";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  const char *sql =
      "create table if not exists BASH_LOGS_TABLE("
      "  ID integer primary key, "
      "  AGENT_NAME text, "
      "  UTC_HOUR integer, "
      "  UTC_MINUTE integer, "
      "  UTC_SECOND integer, "
      "  UTC_DAY integer, "
      "  UTC_MONTH integer, "
      "  UTC_YEAR integer, "
      "  USER_ID integer, "
      "  COMMAND text "
      ");";
  int ret = sqlite_interface_->Exec(db_handle_, sql, nullptr, nullptr, nullptr);
  StatementCheckForError(ret, "Create BASH_LOGS_TABLE error");
}

void Database::CreateApacheLogsTable() {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::CreateApacheLogsTable: Function call";

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::CreateBashLogsTable: Database is not open.";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  const char *sql =
      "create table if not exists APACHE_LOGS_TABLE("
      "  ID integer primary key, "
      "  AGENT_NAME text,"
      "  VIRTUALHOST text, "
      "  CLIENT_IP text, "
      "  UTC_HOUR integer, "
      "  UTC_MINUTE integer, "
      "  UTC_SECOND integer, "
      "  UTC_DAY integer, "
      "  UTC_MONTH integer, "
      "  UTC_YEAR integer, "
      "  REQUEST text, "
      "  STATUS_CODE integer, "
      "  BYTES integer, "
      "  USER_AGENT text "
      ");";
  int ret = sqlite_interface_->Exec(db_handle_, sql, nullptr, nullptr, nullptr);
  StatementCheckForError(ret, "Create APACHE_LOGS_TABLE error");
}

void Database::CreateApacheSessionTable() {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::CreateApacheSessionTable: Function call";

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::CreateApacheSessionTable: Database is not open.";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  const char *sql =
      "create table if not exists APACHE_SESSION_TABLE("
      "  ID integer primary key, "
      "  AGENT_NAME text,"
      "  VIRTUALHOST text, "
      "  CLIENT_IP text, "
      "  SESSION_LENGTH integer, "
      "  BANDWIDTH_USAGE integer, "
      "  REQUESTS_COUNT integer, "
      "  ERROR_PERCENTAGE integer, "
      "  USER_AGENT text "
      ");";
  int ret = sqlite_interface_->Exec(db_handle_, sql, nullptr, nullptr, nullptr);
  StatementCheckForError(ret, "Create APACHE_SESSION_TABLE error");
}

bool Database::AddBashLogs(const type::BashLogs &log_entries) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::AddBashLogs: Function call";

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::AddBashLogs: Database is not open.";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  int ret;
  ret = sqlite_interface_->Exec(db_handle_, "begin transaction", nullptr, nullptr, nullptr);
  StatementCheckForError(ret, "Begin transaction error");

  for (const type::BashLogEntry &entry : log_entries) {
    BOOST_LOG_TRIVIAL(debug) << "database::Database::AddBashLogs: Processing: " << entry.agent_name << " ; " << entry.utc_time << " ; " << entry.user_id << " ; " << entry.command;

    const char *sql = "insert into BASH_LOGS_TABLE(AGENT_NAME, UTC_HOUR, UTC_MINUTE, UTC_SECOND, UTC_DAY, UTC_MONTH, UTC_YEAR, USER_ID, COMMAND) values(?, ?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *statement;
    ret = sqlite_interface_->Prepare(db_handle_, sql, -1, &statement, nullptr);
    StatementCheckForErrorAndRollback(ret, "Prepare insert error");

    ret = sqlite_interface_->BindText(statement, 1, entry.agent_name.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind agentname error");

    ret = sqlite_interface_->BindInt(statement, 2, entry.utc_time.GetHour());
    StatementCheckForErrorAndRollback(ret, "Bind hour error");

    ret = sqlite_interface_->BindInt(statement, 3, entry.utc_time.GetMinute());
    StatementCheckForErrorAndRollback(ret, "Bind minute error");

    ret = sqlite_interface_->BindInt(statement, 4, entry.utc_time.GetSecond());
    StatementCheckForErrorAndRollback(ret, "Bind second error");

    ret = sqlite_interface_->BindInt(statement, 5, entry.utc_time.GetDay());
    StatementCheckForErrorAndRollback(ret, "Bind day error");

    ret = sqlite_interface_->BindInt(statement, 6, entry.utc_time.GetMonth());
    StatementCheckForErrorAndRollback(ret, "Bind month error");

    ret = sqlite_interface_->BindInt(statement, 7, entry.utc_time.GetYear());
    StatementCheckForErrorAndRollback(ret, "Bind year error");

    ret = sqlite_interface_->BindInt(statement, 8, entry.user_id);
    StatementCheckForErrorAndRollback(ret, "Bind user id error");

    ret = sqlite_interface_->BindText(statement, 9, entry.command.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind command error");

    ret = sqlite_interface_->Step(statement);
    if (ret == SQLITE_BUSY) {
      BOOST_LOG_TRIVIAL(error) << "database::Database::AddBashLogs: Step error - SQLite is busy";
      Rollback();
      return false;
    }
    StatementCheckForErrorAndRollback(ret, "Step error");

    ret = sqlite_interface_->Finalize(statement);
    StatementCheckForErrorAndRollback(ret, "Finalize error");
  }

  ret = sqlite_interface_->Exec(db_handle_, "end transaction", nullptr, nullptr, nullptr);
  if (ret == SQLITE_BUSY) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::AddBashLogs: End transaction error - SQLite is busy";
    Rollback();
    return false;
  }
  StatementCheckForErrorAndRollback(ret, "End transaction error");

  return true;
}

bool Database::AddApacheLogs(const type::ApacheLogs &log_entries) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::AddApacheLogs: Function call";

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::AddApacheLogs: Database is not open.";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  int ret;
  ret = sqlite_interface_->Exec(db_handle_, "begin transaction", nullptr, nullptr, nullptr);
  StatementCheckForError(ret, "Begin transaction error");

  for (const type::ApacheLogEntry &entry : log_entries) {
    const char *sql = "insert into APACHE_LOGS_TABLE(AGENT_NAME, VIRTUALHOST, CLIENT_IP, UTC_HOUR, UTC_MINUTE, UTC_SECOND, UTC_DAY, UTC_MONTH, UTC_YEAR, REQUEST, STATUS_CODE, BYTES, USER_AGENT) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *statement;
    ret = sqlite_interface_->Prepare(db_handle_, sql, -1, &statement, nullptr);
    StatementCheckForErrorAndRollback(ret, "Prepare insert error");

    ret = sqlite_interface_->BindText(statement, 1, entry.agent_name.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind agent_name error");

    ret = sqlite_interface_->BindText(statement, 2, entry.virtualhost.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind virtualhost error");

    ret = sqlite_interface_->BindText(statement, 3, entry.client_ip.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind client_ip error");

    ret = sqlite_interface_->BindInt(statement, 4, entry.time.GetHour());
    StatementCheckForErrorAndRollback(ret, "Bind hour error");

    ret = sqlite_interface_->BindInt(statement, 5, entry.time.GetMinute());
    StatementCheckForErrorAndRollback(ret, "Bind minute error");

    ret = sqlite_interface_->BindInt(statement, 6, entry.time.GetSecond());
    StatementCheckForErrorAndRollback(ret, "Bind second error");

    ret = sqlite_interface_->BindInt(statement, 7, entry.time.GetDay());
    StatementCheckForErrorAndRollback(ret, "Bind day error");

    ret = sqlite_interface_->BindInt(statement, 8, entry.time.GetMonth());
    StatementCheckForErrorAndRollback(ret, "Bind month error");

    ret = sqlite_interface_->BindInt(statement, 9, entry.time.GetYear());
    StatementCheckForErrorAndRollback(ret, "Bind year error");

    ret = sqlite_interface_->BindText(statement, 10, entry.request.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind request error");

    ret = sqlite_interface_->BindInt(statement, 11, entry.status_code);
    StatementCheckForErrorAndRollback(ret, "Bind status_code id error");

    ret = sqlite_interface_->BindInt(statement, 12, entry.bytes);
    StatementCheckForErrorAndRollback(ret, "Bind bytes error");

    ret = sqlite_interface_->BindText(statement, 13, entry.user_agent.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind user_agent error");

    ret = sqlite_interface_->Step(statement);
    if (ret == SQLITE_BUSY) {
      BOOST_LOG_TRIVIAL(error) << "database::Database::AddBashLogs: Step error - SQLite is busy";
      Rollback();
      return false;
    }
    StatementCheckForErrorAndRollback(ret, "Step error");

    ret = sqlite_interface_->Finalize(statement);
    StatementCheckForErrorAndRollback(ret, "Finalize error");
  }

  ret = sqlite_interface_->Exec(db_handle_, "end transaction", nullptr, nullptr, nullptr);
  if (ret == SQLITE_BUSY) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::AddBashLogs: End transaction error - SQLite is busy";
    Rollback();
    return false;
  }
  StatementCheckForErrorAndRollback(ret, "End transaction error");

  return true;
}

bool Database::AddApacheSessionStatistics(const analyzer::ApacheSessions &sessions) {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::AddApacheSessionStatistics: Function call";

  if (is_open_ == false) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::AddApacheSessionStatistics: Database is not open.";
    throw exception::detail::CantExecuteSqlStatementException();
  }

  int ret;
  ret = sqlite_interface_->Exec(db_handle_, "begin transaction", nullptr, nullptr, nullptr);
  StatementCheckForError(ret, "Begin transaction error");

  for (const analyzer::ApacheSessionEntry &entry : sessions) {
    const char *sql = "insert into APACHE_SESSION_TABLE(AGENT_NAME, VIRTUALHOST, CLIENT_IP, SESSION_LENGTH, BANDWIDTH_USAGE, REQUESTS_COUNT, ERROR_PERCENTAGE, USER_AGENT) values(?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *statement;
    ret = sqlite_interface_->Prepare(db_handle_, sql, -1, &statement, nullptr);
    StatementCheckForErrorAndRollback(ret, "Prepare insert error");

    ret = sqlite_interface_->BindText(statement, 1, entry.agent_name.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind agent_name error");

    ret = sqlite_interface_->BindText(statement, 2, entry.virtualhost.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind virtualhost error");

    ret = sqlite_interface_->BindText(statement, 3, entry.client_ip.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind client_ip error");

    ret = sqlite_interface_->BindInt64(statement, 4, entry.session_length);
    StatementCheckForErrorAndRollback(ret, "Bind session_length error");
    
    ret = sqlite_interface_->BindInt64(statement, 5, entry.bandwidth_usage);
    StatementCheckForErrorAndRollback(ret, "Bind bandwidth_usage error");
    
    ret = sqlite_interface_->BindInt64(statement, 6, entry.requests_count);
    StatementCheckForErrorAndRollback(ret, "Bind requests_count error");
    
    ret = sqlite_interface_->BindInt(statement, 7, entry.error_percentage);
    StatementCheckForErrorAndRollback(ret, "Bind error_percentage error");

    ret = sqlite_interface_->BindText(statement, 8, entry.useragent.c_str(), -1, nullptr);
    StatementCheckForErrorAndRollback(ret, "Bind useragent error");

    ret = sqlite_interface_->Step(statement);
    if (ret == SQLITE_BUSY) {
      BOOST_LOG_TRIVIAL(error) << "database::Database::AddApacheSessionStatistics: Step error - SQLite is busy";
      Rollback();
      return false;
    }
    StatementCheckForErrorAndRollback(ret, "Step error");

    ret = sqlite_interface_->Finalize(statement);
    StatementCheckForErrorAndRollback(ret, "Finalize error");
  }

  ret = sqlite_interface_->Exec(db_handle_, "end transaction", nullptr, nullptr, nullptr);
  if (ret == SQLITE_BUSY) {
    BOOST_LOG_TRIVIAL(error) << "database::Database::AddBashLogs: End transaction error - SQLite is busy";
    Rollback();
    return false;
  }
  StatementCheckForErrorAndRollback(ret, "End transaction error");

  return true;
}

bool Database::Close() {
  BOOST_LOG_TRIVIAL(debug) << "database::Database::Close: Function call";

  if (is_open_) {
    int ret = sqlite_interface_->Close(db_handle_);
    if (ret != SQLITE_OK) {
      BOOST_LOG_TRIVIAL(error) << "database::Database::Close: Failed to close database: " << ret;
      throw exception::detail::CantCloseDatabaseException();
    }

    is_open_ = false;
    return true;
  }
  else {
    BOOST_LOG_TRIVIAL(warning) << "database::Database::Close: Database already closed";
    return false;
  }
}

Database::Database(std::unique_ptr<database::detail::SQLiteInterface> sqlite)
: is_open_(false),
db_handle_(nullptr),
sqlite_interface_(std::move(sqlite)) {
}

void Database::StatementCheckForError(int return_value, const char *description) {
  if (return_value != SQLITE_OK && return_value != SQLITE_DONE) {
    BOOST_LOG_TRIVIAL(error) << "database::Database: " << description << ": " << return_value;
    throw exception::detail::CantExecuteSqlStatementException();
  }
}

void Database::StatementCheckForErrorAndRollback(int return_value, const char *description) {
  if (return_value != SQLITE_OK && return_value != SQLITE_DONE) {
    BOOST_LOG_TRIVIAL(error) << "database::Database: " << description << ": " << return_value;
    Rollback();
    throw exception::detail::CantExecuteSqlStatementException();
  }
}

void Database::Rollback() {
  int ret = sqlite_interface_->Exec(db_handle_, "rollback", nullptr, nullptr, nullptr);
  if (ret != SQLITE_OK) {
    BOOST_LOG_TRIVIAL(error) << "database::Database: Rollback error: " << ret;
    throw exception::detail::CantExecuteSqlStatementException();
  }
}

}
