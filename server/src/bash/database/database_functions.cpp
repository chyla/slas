/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "database_functions.h"

#include <boost/log/trivial.hpp>
#include <string>

#include "type/uid.h"
#include "detail/raw_database_functions.h"

using namespace std;

namespace bash
{

namespace database
{

DatabaseFunctionsPtr DatabaseFunctions::Create(::database::detail::SQLiteWrapperInterfacePtr sqlite_wrapper,
                                               ::database::detail::GeneralDatabaseFunctionsInterfacePtr general_database_functions) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::Create: Function call";

  auto raw_database_functions = detail::RawDatabaseFunctions::Create(sqlite_wrapper);

  return Create(raw_database_functions, general_database_functions);
}

DatabaseFunctionsPtr DatabaseFunctions::Create(::bash::database::detail::RawDatabaseFunctionsInterfacePtr raw_database_functions,
                                               ::database::detail::GeneralDatabaseFunctionsInterfacePtr general_database_functions) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::Create: Function call";

  return DatabaseFunctionsPtr(new DatabaseFunctions(raw_database_functions, general_database_functions));
}

void DatabaseFunctions::CreateTables() {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::CreateTables: Function call";

  raw_database_functions_->CreateTables();
}

void DatabaseFunctions::AddSystemUser(::bash::database::type::UID uid) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddSystemUser: Function call";

  raw_database_functions_->AddSystemUser({uid});
}

::database::type::RowId DatabaseFunctions::GetSystemUserId(::bash::database::type::UID uid) {
  BOOST_LOG_TRIVIAL(debug) << "database::DatabaseFunctions::GetSystemUserId: Function call";

  return raw_database_functions_->GetSystemUserId({uid});
}

::bash::database::detail::entity::SystemUser DatabaseFunctions::GetSystemUserById(::database::type::RowId id) {
  BOOST_LOG_TRIVIAL(debug) << "database::DatabaseFunctions::GetSystemUserById: Function call";

  return raw_database_functions_->GetSystemUserById(id);
}

::database::type::RowIds DatabaseFunctions::GetSystemUsersIdsFromLogs(::database::type::RowId agent_name_id) {
  BOOST_LOG_TRIVIAL(debug) << "database::DatabaseFunctions::GetSystemUsersIdsFromLogs: Function call";

  return raw_database_functions_->GetSystemUsersIdsFromLogs(agent_name_id);
}

void DatabaseFunctions::AddCommand(const ::bash::database::type::CommandName &command) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddCommand: Function call";

  return raw_database_functions_->AddCommand(command);
}

::database::type::RowId DatabaseFunctions::GetCommandId(const ::bash::database::type::CommandName &command) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetCommandId: Function call";

  return raw_database_functions_->GetCommandId(command);
}

::database::type::RowIds DatabaseFunctions::GetAllCommandsIds() {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetAllCommandsIds: Function call";

  return raw_database_functions_->GetAllCommandsIds();
}

::bash::database::type::CommandName DatabaseFunctions::GetCommandNameById(::database::type::RowId id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetCommandNameById: Function call";

  return raw_database_functions_->GetCommandNameById(id);
}

void DatabaseFunctions::AddLog(const ::type::BashLogEntry &log_entry) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddLog: Function call";

  detail::entity::Log raw_log;
  raw_log.agent_name_id = general_database_functions_->GetAgentNameId(log_entry.agent_name);
  raw_log.date_id = general_database_functions_->GetDateId(log_entry.utc_time.GetDate());
  raw_log.time_id = general_database_functions_->GetTimeId(log_entry.utc_time.GetTime());
  raw_log.user_id = GetSystemUserId(log_entry.user_id);
  raw_log.command_id = raw_database_functions_->GetCommandId(log_entry.command);

  raw_database_functions_->AddLog(raw_log);
}

::database::type::RowsCount DatabaseFunctions::CountCommandsForDailySystemStatistic(::database::type::RowId agent_name_id,
                                                                                    ::database::type::RowId date_id,
                                                                                    ::database::type::RowId command_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::CountCommandsForDailySystemStatistic: Function call";

  return raw_database_functions_->CountCommandsForDailySystemStatistic(agent_name_id, date_id, command_id);
}

::database::type::RowsCount DatabaseFunctions::CountCommandsForUserDailyStatisticFromLogs(::database::type::RowId agent_name_id,
                                                                                          ::database::type::RowId date_id,
                                                                                          ::database::type::RowId user_id,
                                                                                          ::database::type::RowId command_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::CountCommandsForUserDailyStatisticFromLogs: Function call";

  return raw_database_functions_->CountCommandsForUserDailyStatisticFromLogs(agent_name_id, date_id, user_id, command_id);
}

void DatabaseFunctions::AddDailySystemStatistic(const detail::entity::DailySystemStatistic &statistics) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddDailySystemStatistic: Function call";

  raw_database_functions_->AddDailySystemStatistic(statistics);
}

void DatabaseFunctions::AddDailySystemStatistics(const detail::entity::DailySystemStatistics &statistics) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddDailySystemStatistic: Function call";

  raw_database_functions_->AddDailySystemStatistics(statistics);
}

::database::type::RowIds DatabaseFunctions::GetDateIdsWithoutCreatedDailySystemStatistic(::database::type::RowId agent_name_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetDateIdsWithoutCreatedDailySystemStatistic: Function call";

  return raw_database_functions_->GetDateIdsWithoutCreatedDailySystemStatistic(agent_name_id);
}

::database::type::RowIds DatabaseFunctions::GetAgentIdsWithoutConfiguration() {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetAgentIdsWithoutConfiguration: Function call";

  return raw_database_functions_->GetAgentIdsWithoutConfiguration();
}

::database::type::RowIds DatabaseFunctions::GetAgentsIdsWithConfiguration() {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetAgentsIdsWithConfigurations: Function call";

  return raw_database_functions_->GetAgentsIdsWithConfiguration();
}

void DatabaseFunctions::AddDailyUserStatistic(const ::bash::database::detail::entity::DailyUserStatistic &us) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddDailyUserStatistic: Function call";

  raw_database_functions_->AddDailyUserStatistic(us);
}

::database::entity::AgentNames DatabaseFunctions::GetAgentsWithExistingDailyUserStatistics() {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetAgentsWithExistingDailyUserStatistics: Function call";

  return raw_database_functions_->GetAgentsWithExistingDailyUserStatistics();
}

::database::entity::AgentNames DatabaseFunctions::GetAgentsWithExistingDailyUserStatisticsNotInLearningSet() {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetAgentsWithExistingDailyUserStatisticsNotInLearningSet: Function call";

  return raw_database_functions_->GetAgentsWithExistingDailyUserStatisticsNotInLearningSet();
}

::database::type::RowsCount DatabaseFunctions::CountDailyUserStatisticsForAgentWithClassification(::database::type::RowId agent_name_id,
                                                                                                  ::database::type::Classification classification) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::CountDailyUserStatisticsForAgentWithClassification: Function call";

  return raw_database_functions_->CountDailyUserStatisticsForAgentWithClassification(agent_name_id, classification);
}

::bash::database::detail::entity::DailyUserStatistics DatabaseFunctions::GetDailyUserStatisticsForAgentWithClassification(::database::type::RowId agent_name_id,
                                                                                                                          ::database::type::Classification classification,
                                                                                                                          ::database::type::RowsCount limit,
                                                                                                                          ::database::type::RowsCount offset) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetDailyUserStatisticsForAgentWithClassification: Function call";

  return raw_database_functions_->GetDailyUserStatisticsForAgentWithClassification(agent_name_id, classification, limit, offset);
}

::database::type::RowId DatabaseFunctions::GetDailyUserStatisticId(::database::type::RowId agent_name_id,
                                                                   ::database::type::RowId user_id,
                                                                   ::database::type::RowId date_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetDailyUserStatisticId: Function call";

  return raw_database_functions_->GetDailyUserStatisticId(agent_name_id, user_id, date_id);
}

::bash::database::detail::entity::DailyUserStatistic DatabaseFunctions::GetDailyUserStatisticById(const ::database::type::RowId &id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetDailyUserStatisticById: Function call";

  return raw_database_functions_->GetDailyUserStatisticById(id);
}

void DatabaseFunctions::SetDailyUserStatisticsClassification(const ::database::type::RowIds &ids, ::database::type::Classification classification) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::SetDailyUserStatisticsClassification: Function call";

  return raw_database_functions_->SetDailyUserStatisticsClassification(ids, classification);
}

void DatabaseFunctions::AddDailyUserCommandStatistic(const ::bash::database::detail::entity::DailyUserCommandStatistic &ucs) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetDailyUserStatisticId: Function call";

  return raw_database_functions_->AddDailyUserCommandStatistic(ucs);
}

::bash::database::type::AnomalyDetectionConfigurations DatabaseFunctions::GetAnomalyDetectionConfigurations() {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetAnomalyDetectionConfigurations: Function call";

  auto raw_configs = raw_database_functions_->GetAnomalyDetectionConfigurations();

  ::bash::database::type::AnomalyDetectionConfigurations configs;
  ::bash::database::type::AnomalyDetectionConfiguration c;

  for (const auto &config : raw_configs) {
    c.id = config.id;
    c.agent_name_id = config.agent_name_id;
    c.begin_date = general_database_functions_->GetDateById(config.begin_date_id);
    c.end_date = general_database_functions_->GetDateById(config.end_date_id);
    c.changed = config.changed;

    configs.push_back(c);
  }

  return configs;
}

void DatabaseFunctions::RemoveAnomalyDetectionConfiguration(::database::type::RowId id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::RemoveAnomalyDetectionConfiguration: Function call";

  raw_database_functions_->RemoveAnomalyDetectionConfiguration(id);
}

void DatabaseFunctions::AddAnomalyDetectionConfiguration(const ::bash::database::type::AnomalyDetectionConfiguration &configuration) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddAnomalyDetectionConfiguration: Function call";

  detail::entity::AnomalyDetectionConfiguration c;
  c.agent_name_id = configuration.agent_name_id;
  c.begin_date_id = general_database_functions_->GetDateId(configuration.begin_date);
  c.end_date_id = general_database_functions_->GetDateId(configuration.end_date);
  c.changed = configuration.changed;

  raw_database_functions_->AddAnomalyDetectionConfiguration(c);
}

::database::type::RowId DatabaseFunctions::GetConfigurationIdForAgent(::database::type::RowId agent_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetConfigurationIdForAgent: Function call";

  return raw_database_functions_->GetConfigurationIdForAgent(agent_id);
}

void DatabaseFunctions::RemoveAllCommandsFromConfiguration(::database::type::RowId configuration_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::RemoveAllCommandsFromConfiguration: Function call";

  raw_database_functions_->RemoveAllCommandsFromConfiguration(configuration_id);
}

void DatabaseFunctions::AddDefaultCommandsToConfiguration(::database::type::RowId configuration_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddDefaultCommandsToConfiguration: Function call";

  raw_database_functions_->AddDefaultCommandsToConfiguration(configuration_id);
}

void DatabaseFunctions::MarkConfigurationAsUnchanged(::database::type::RowId configuration_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::MarkConfigurationAsUnchanged: Function call";

  raw_database_functions_->MarkConfigurationAsUnchanged(configuration_id);
}

void DatabaseFunctions::MarkConfigurationAsChanged(::database::type::RowId configuration_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::MarkConfigurationAsChanged: Function call";

  raw_database_functions_->MarkConfigurationAsChanged(configuration_id);
}

void DatabaseFunctions::AddCommandStatistic(const detail::entity::CommandStatistic &statistic) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddCommandStatistic: Function call";

  raw_database_functions_->AddCommandStatistic(statistic);
}

void DatabaseFunctions::AddCommandsStatistics(const detail::entity::CommandsStatistics &statistics) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddCommandsStatistics: Function call";

  raw_database_functions_->AddCommandsStatistics(statistics);
}

bool DatabaseFunctions::IsCommandStatisticExist(::database::type::RowId agent_name_id,
                                                ::database::type::RowId command_id,
                                                ::database::type::RowId begin_date_id,
                                                ::database::type::RowId end_date_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::IsCommandStatisticExist: Function call";

  return raw_database_functions_->IsCommandStatisticExist(agent_name_id, command_id, begin_date_id, end_date_id);
}

detail::entity::CommandsStatistics DatabaseFunctions::GetCommandsStatistics(::database::type::RowId agent_name_id,
                                                                            ::database::type::RowId begin_date_id,
                                                                            ::database::type::RowId end_date_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetCommandStatistic: Function call";

  return raw_database_functions_->GetCommandsStatistics(agent_name_id, begin_date_id, end_date_id);
}

detail::entity::CommandsStatistics DatabaseFunctions::GetCommandsStatistics(::database::type::RowId configuration_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetCommandStatistic: Function call";

  return raw_database_functions_->GetCommandsStatistics(configuration_id);
}

::database::type::RowIds DatabaseFunctions::GetMarkedCommandsIds(::database::type::RowId configuration_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetMarkedCommandsIds: Function call";

  return raw_database_functions_->GetMarkedCommandsIds(configuration_id);
}

::database::type::RowsCount DatabaseFunctions::CommandSummary(::database::type::RowId agent_name_id,
                                                              ::database::type::RowId command_id,
                                                              ::database::type::RowIds date_range_ids) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::CommandSummary: Function call";

  return raw_database_functions_->CommandSummary(agent_name_id, command_id, date_range_ids);
}

void DatabaseFunctions::AddSelectedCommandsIds(::database::type::RowId configuration_id,
                                               ::database::type::RowIds command_names_ids) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddSelectedCommandsIds: Function call";

  raw_database_functions_->AddSelectedCommandsIds(configuration_id, command_names_ids);
}

::database::type::RowIds DatabaseFunctions::GetNotCalculatedDatesIdsFromLogs(::database::type::RowId agent_name_id,
                                                                             ::database::type::RowId user_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetNotCalculatedDatesIdsFromLogs: Function call";

  return raw_database_functions_->GetNotCalculatedDatesIdsFromLogs(agent_name_id, user_id);
}

::database::type::RowIds DatabaseFunctions::GetCommandsIdsFromLogs(::database::type::RowId agent_name_id,
                                                                   ::database::type::RowId user_id,
                                                                   ::database::type::RowId date_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetCommandsIdsFromLogs: Function call";

  return raw_database_functions_->GetCommandsIdsFromLogs(agent_name_id, user_id, date_id);
}

void DatabaseFunctions::AddDailyUserStatisticsToConfiguration(::database::type::RowId configuration_id,
                                                              const ::database::type::RowIds &date_range_ids) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::AddDailyUserStatisticsToConfiguration: Function call";

  raw_database_functions_->AddDailyUserStatisticsToConfiguration(configuration_id, date_range_ids);
}

void DatabaseFunctions::RemoveDailyStatisticsFromConfiguration(::database::type::RowId configuration_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::RemoveDailyStatisticsFromConfiguration: Function call";

  raw_database_functions_->RemoveDailyStatisticsFromConfiguration(configuration_id);
}

::bash::database::detail::entity::DailyUserStatistics DatabaseFunctions::GetDailyUserStatisticsFromConfiguration(::database::type::RowId configuration_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetDailyUserStatisticsFromConfiguration: Function call";

  return raw_database_functions_->GetDailyUserStatisticsFromConfiguration(configuration_id);
}

::bash::database::detail::entity::DailyUserStatistics DatabaseFunctions::GetDailyUserStatisticsForAgent(::database::type::RowId agent_name_id,
                                                                                                        const ::database::type::RowIds &date_range_ids) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetDailyUserStatisticsForAgent: Function call";

  return raw_database_functions_->GetDailyUserStatisticsForAgent(agent_name_id, date_range_ids);
}

::bash::database::detail::entity::DailyUserStatistics DatabaseFunctions::GetDailyUserStatisticsWithoutLearningSetForAgent(::database::type::RowId agent_name_id,
                                                                                                                          const ::database::type::RowIds &date_range_ids) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetDailyUserStatisticsWithoutLearningSetForAgent: Function call";

  return raw_database_functions_->GetDailyUserStatisticsWithoutLearningSetForAgent(agent_name_id, date_range_ids);
}

::database::type::RowIds DatabaseFunctions::GetUsersIdsFromSelectedDailyStatisticsInConfiguration(::database::type::RowId configuration_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetUsersIdsFromDailyStatistics: Function call";

  return raw_database_functions_->GetUsersIdsFromSelectedDailyStatisticsInConfiguration(configuration_id);
}

::database::type::RowId DatabaseFunctions::CountSelectedDailyStatisticsWithoutUnknownClassificationInConfiguration(::database::type::RowId configuration_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::CountSelectedDailyStatisticsWithoutUnknownClassificationInConfiguration: Function call";

  return raw_database_functions_->CountSelectedDailyStatisticsWithoutUnknownClassificationInConfiguration(configuration_id);
}

::database::type::RowsCount DatabaseFunctions::CountSelectedDailyUserStatisticsWithoutUnknownClassificationFromConfigurationByUser(::database::type::RowId configuration_id,
                                                                                                                                   ::database::type::RowId user_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::CountSelectedDailyUserStatisticsWithoutUnknownClassificationFromConfigurationByUser: Function call";

  return raw_database_functions_->CountSelectedDailyUserStatisticsWithoutUnknownClassificationFromConfigurationByUser(configuration_id, user_id);
}

::bash::database::detail::entity::DailyUserStatistics DatabaseFunctions::GetSelectedDailyUserStatisticsWithoutUnknownClassificationFromConfigurationByUser(::database::type::RowId configuration_id,
                                                                                                                                                           ::database::type::RowId user_id,
                                                                                                                                                           ::database::type::RowsCount limit,
                                                                                                                                                           ::database::type::RowsCount offset) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetSelectedDailyUserStatisticsWithoutUnknownClassificationFromConfigurationByUser: Function call";

  return raw_database_functions_->GetSelectedDailyUserStatisticsWithoutUnknownClassificationFromConfigurationByUser(configuration_id, user_id, limit, offset);
}

::bash::database::detail::entity::DailyUserCommandsStatistics DatabaseFunctions::GetSelectedDailyUserCommandsStatistics(::database::type::RowId statistic_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetSelectedDailyUserCommandsStatistics: Function call";

  return raw_database_functions_->GetSelectedDailyUserCommandsStatistics(statistic_id);
}

::bash::database::detail::type::DailyUserNamedCommandsStatistics DatabaseFunctions::GetDailyUserNamedCommandsStatistics(::database::type::RowId daily_user_statistic_id) {
  BOOST_LOG_TRIVIAL(debug) << "bash::database::DatabaseFunctions::GetDailyUserNamedCommandsStatistics: Function call";

  return raw_database_functions_->GetDailyUserNamedCommandsStatistics(daily_user_statistic_id);
}

DatabaseFunctions::DatabaseFunctions(::bash::database::detail::RawDatabaseFunctionsInterfacePtr raw_database_functions,
                                     ::database::detail::GeneralDatabaseFunctionsInterfacePtr general_database_functions) :
raw_database_functions_(raw_database_functions),
general_database_functions_(general_database_functions) {
}

}

}
