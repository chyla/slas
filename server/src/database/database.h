#pragma once

#include <string>
#include <memory>
#include <vector>

#include <slas/type/time.h>
#include <slas/type/date.h>
#include <slas/type/timestamp.h>
#include <slas/type/apache_log_entry.h>
#include <slas/type/bash_log_entry.h>

#include "type/row_id.h"
#include "type/agent_name.h"
#include "type/virtualhost_name.h"
#include "detail/sqlite_interface.h"
#include "src/apache/type/apache_session_entry.h"
#include "src/apache/type/anomaly_detection_configuration_entry.h"

namespace database
{

class Database;
typedef std::shared_ptr<Database> DatabasePtr;

class Database {
 public:
  static DatabasePtr Create();
  static DatabasePtr Create(std::unique_ptr<detail::SQLiteInterface> sqlite);

  void Open(sqlite3 *db_handle);

  bool IsOpen() const;

  ::type::Date GetDateById(type::RowId id) __attribute__((deprecated));

  bool AddApacheSessionStatistics(const ::apache::type::ApacheSessions &sessions);

  long long GetApacheSessionStatisticsCount(const std::string &agent_name, const std::string &virtualhost_name,
                                            const ::type::Timestamp &from, const ::type::Timestamp &to);

  ::apache::type::ApacheSessions GetApacheSessionStatistics(const std::string &agent_name, const std::string &virtualhost_name,
                                                            const ::type::Timestamp &from, const ::type::Timestamp &to,
                                                            unsigned limit, long long offset);

  void SetApacheSessionAsAnomaly(type::RowIds all, type::RowIds anomaly);

  ::apache::type::ApacheSessionEntry GetApacheOneSessionStatistic(long long id);

  void SetApacheAnomalyDetectionConfiguration(const ::apache::type::AnomalyDetectionConfigurationEntry &configuration);

  const ::apache::type::AnomalyDetectionConfiguration GetApacheAnomalyDetectionConfiguration();

  type::AgentNames GetApacheAgentNames();

  type::VirtualhostNames GetApacheVirtualhostNames(std::string agent_name);

 private:
  bool is_open_;
  sqlite3 *db_handle_;
  std::unique_ptr<detail::SQLiteInterface> sqlite_interface_;

  Database(std::unique_ptr<detail::SQLiteInterface> sqlite);

  void StatementCheckForError(int return_value, const char *description);
  void StatementCheckForErrorAndRollback(int return_value, const char *description);
  void Rollback();
  static int GetApacheAgentNamesCallback(void *names_vptr, int argc, char **argv, char **azColName);
  std::string GetTimeRule(const ::type::Timestamp &from, const ::type::Timestamp &to) const;
  std::string TextHelper(unsigned const char *text) const;
  long long GetApacheCount(const std::string &table, const std::string &agent_name,
                           const std::string &virtualhost_name, const ::type::Timestamp &from,
                           const ::type::Timestamp &to);
};

}
