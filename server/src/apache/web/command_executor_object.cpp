#include "command_executor_object.h"

#include <boost/log/trivial.hpp>
#include <json/json.hpp>
#include <algorithm>
#include <vector>
#include <string>
#include <slas/type/time.h>

using namespace std;
using namespace nlohmann;

namespace apache
{

namespace web
{

CommandExecutorObjectPtr CommandExecutorObject::Create(::database::DatabasePtr database,
                                                       ::database::detail::GeneralDatabaseFunctionsInterfacePtr general_database_functions,
                                                       ::apache::database::detail::DatabaseFunctionsInterfacePtr apache_database_functions) {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::Create: Function call";
  auto p = CommandExecutorObjectPtr(new CommandExecutorObject(database, general_database_functions, apache_database_functions));
  return p;
}

const ::web::type::JsonMessage CommandExecutorObject::Execute(const ::web::type::JsonMessage &message) {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::Execute: Function call";
  auto json_object = json::parse(message);
  auto command = json_object["command"];
  auto result = GetUnknownCommandErrorJson();

  if (command == "get_apache_agent_names") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'get_apache_agent_names' command";
    result = GetHostnames();
  }
  else if (command == "get_apache_virtualhosts_names") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'get_apache_virtualhosts_names' command";

    auto args = json_object["args"];
    if (args.size() != 1) {
      BOOST_LOG_TRIVIAL(warning) << "apache::web::CommandExecutorObject::Execute: get_apache_virtualhosts_names require one argument";
      return GetInvalidArgumentErrorJson();
    }

    auto agent_name = args.at(0);
    result = GetVirtualhostsNames(agent_name);
  }
  else if (command == "get_apache_sessions") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'get_apache_sessions' command";

    auto args = json_object["args"];
    if (args.size() != 4) {
      BOOST_LOG_TRIVIAL(warning) << "apache::web::CommandExecutorObject::Execute: get_apache_sessions require four arguments";
      return GetInvalidArgumentErrorJson();
    }

    result = GetSessions(args.at(0), args.at(1), args.at(2), args.at(3));
  }
  else if (command == "set_apache_sessions_as_anomaly") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'set_apache_sessions_as_anomaly' command";

    auto args = json_object["args"];
    if (args.size() != 4) {
      BOOST_LOG_TRIVIAL(warning) << "apache::web::CommandExecutorObject::Execute: set_apache_sessions_as_anomaly require two arguments";
      return GetInvalidArgumentErrorJson();
    }

    result = SetApacheSessionsAsAnomaly(args.at(0), args.at(1), args.at(2), args.at(3));
  }
  else if (command == "get_apache_anomaly_detection_configuration") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'get_apache_anomaly_detection_configuration' command";

    result = GetApacheAnomalyDetectionConfiguration();
  }
  else if (command == "set_apache_anomaly_detection_configuration") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'set_apache_anomaly_detection_configuration' command";

    auto args = json_object["args"];
    if (args.size() != 4) {
      BOOST_LOG_TRIVIAL(warning) << "apache::web::CommandExecutorObject::Execute: set_apache_anomaly_detection_configuration require four arguments";
      return GetInvalidArgumentErrorJson();
    }

    result = SetApacheAnomalyDetectionConfiguration(args.at(0), args.at(1), args.at(2), args.at(3));
  }
  else if (command == "get_apache_sessions_without_learning_set") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'get_apache_sessions_without_learning_set' command";

    auto args = json_object["args"];
    if (args.size() != 4) {
      BOOST_LOG_TRIVIAL(warning) << "apache::web::CommandExecutorObject::Execute: get_apache_sessions_without_learning_set require four arguments";
      return GetInvalidArgumentErrorJson();
    }

    result = GetSessionsWithoutLearningSet(args.at(0), args.at(1), args.at(2), args.at(3));
  }
  else if (command == "mark_learning_set_with_iqr_method") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'mark_learning_set_with_iqr_method' command";

    auto args = json_object["args"];
    if (args.size() != 2) {
      BOOST_LOG_TRIVIAL(warning) << "apache::web::CommandExecutorObject::Execute: mark_learning_set_with_iqr_method requires two arguments";
      return GetInvalidArgumentErrorJson();
    }

    result = MarkLearningSetWithIqrMethod(args.at(0), args.at(1));
  }
  else if (command == "get_agents_and_virtualhosts_names_filtered_by_sessions_classification_exists") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'get_agents_and_virtualhosts_names_filtered_by_sessions_classification_exists' command";

    result = GetAgentsAndVirtualhostsNamesFilteredBySessionsClassificationExists();
  }
  else if (command == "get_learning_set_sessions") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'get_learning_set_sessions' command";

    auto args = json_object["args"];
    if (args.size() != 2) {
      BOOST_LOG_TRIVIAL(warning) << "apache::web::CommandExecutorObject::Execute: get_learning_set_sessions requires two arguments";
      return GetInvalidArgumentErrorJson();
    }

    result = GetLearningSetSessions(args.at(0), args.at(1));
  }
  else if (command == "remove_configuration") {
    BOOST_LOG_TRIVIAL(info) << "apache::web::CommandExecutorObject::Execute: Found 'remove_configuration' command";

    auto args = json_object["args"];
    if (args.size() != 1) {
      BOOST_LOG_TRIVIAL(warning) << "apache::web::CommandExecutorObject::Execute: remove_configuration require one argument";
      return GetInvalidArgumentErrorJson();
    }

    result = RemoveConfiguration(args.at(0));
  }

  return result;
}

bool CommandExecutorObject::IsCommandSupported(const ::web::type::Command &command) {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::IsCommandSupported: Function call";
  return (command == "get_apache_agent_names")
      || (command == "get_apache_virtualhosts_names")
      || (command == "get_apache_sessions")
      || (command == "set_apache_sessions_as_anomaly")
      || (command == "get_apache_anomaly_detection_configuration")
      || (command == "set_apache_anomaly_detection_configuration")
      || (command == "get_apache_sessions_without_learning_set")
      || (command == "mark_learning_set_with_iqr_method")
      || (command == "get_agents_and_virtualhosts_names_filtered_by_sessions_classification_exists")
      || (command == "get_learning_set_sessions")
      || (command == "remove_configuration")
      ;
}

CommandExecutorObject::CommandExecutorObject(::database::DatabasePtr database,
                                             ::database::detail::GeneralDatabaseFunctionsInterfacePtr general_database_functions,
                                             ::apache::database::detail::DatabaseFunctionsInterfacePtr apache_database_functions) :
database_(database),
general_database_functions_(general_database_functions),
apache_database_functions_(apache_database_functions) {
}

const ::web::type::JsonMessage CommandExecutorObject::GetHostnames() {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::GetHostnames: Function call";

  json j;
  j["status"] = "ok";
  j["result"] = database_->GetApacheAgentNames();

  return j.dump();
}

const ::web::type::JsonMessage CommandExecutorObject::GetVirtualhostsNames(const std::string &agent_name) {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::GetVirtualhostsNames: Function call";

  json j;
  j["status"] = "ok";
  j["result"] = database_->GetApacheVirtualhostNames(agent_name);

  return j.dump();
}

const ::web::type::JsonMessage CommandExecutorObject::GetSessions(const std::string &agent_name,
                                                                  const std::string &virtualhost_name,
                                                                  const std::string &begin_date,
                                                                  const std::string &end_date) {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::GetSessions: Function call";

  auto tbegin = ::type::Timestamp::Create(::type::Time(),
                                          ::type::Date::Create(begin_date));
  auto tend = ::type::Timestamp::Create(::type::Time::Create(23, 59, 59),
                                        ::type::Date::Create(end_date));
  auto count = database_->GetApacheSessionStatisticsCount(agent_name, virtualhost_name,
                                                          tbegin, tend);

  ::apache::type::ApacheSessions sessions = database_->GetApacheSessionStatistics(agent_name, virtualhost_name,
                                                                                  tbegin, tend,
                                                                                  count, 0);

  json j, r = json::array();
  for (::apache::type::ApacheSessionEntry s : sessions) {
    json t;
    t["id"] = s.id;
    t["agent_name"] = s.agent_name;
    t["virtualhost"] = s.virtualhost;
    t["client_ip"] = s.client_ip;
    t["session_start"] = s.session_start.ToString();
    t["session_length"] = s.session_length;
    t["bandwidth_usage"] = s.bandwidth_usage;
    t["requests_count"] = s.requests_count;
    t["error_percentage"] = s.error_percentage;
    t["useragent"] = s.useragent;
    t["classification"] = static_cast<int>(s.classification);

    r.push_back(t);
  }

  j["status"] = "ok";
  j["result"] = r;

  return j.dump();
}

const ::web::type::JsonMessage CommandExecutorObject::SetApacheSessionsAsAnomaly(const ::database::type::AgentName &agent_name,
                                                                                 const ::database::type::VirtualhostName &virtualhost_name,
                                                                                 const std::vector<long long> &all,
                                                                                 const std::vector<long long> &anomalies) {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::SetApacheSessionsAsAnomaly: Function call";

  database_->SetApacheSessionAsAnomaly(all, anomalies);

  auto agent_id = general_database_functions_->GetAgentNameId(agent_name);
  auto virtualhost_id = apache_database_functions_->GetVirtualhostNameId(virtualhost_name);

  apache_database_functions_->RemoveAllLearningSessions(agent_id, virtualhost_id);
  apache_database_functions_->SetLearningSessions(agent_id, virtualhost_id, all);

  json j;
  j["status"] = "ok";

  return j.dump();
}

const ::web::type::JsonMessage CommandExecutorObject::GetApacheAnomalyDetectionConfiguration() {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::GetApacheAnomalyDetectionConfiguration: Function call";

  auto conf = database_->GetApacheAnomalyDetectionConfiguration();

  json r = json::array();
  for (auto c : conf) {
    json t;
    t["id"] = c.id;
    t["agent_name"] = c.agent_name;
    t["virtualhost_name"] = c.virtualhost_name;
    t["begin_date"] = c.begin_date.ToString();
    t["end_date"] = c.end_date.ToString();

    r.push_back(t);
  }

  json j;
  j["status"] = "ok";
  j["result"] = r;

  return j.dump();
}

const ::web::type::JsonMessage CommandExecutorObject::SetApacheAnomalyDetectionConfiguration(const std::string &agent_name,
                                                                                             const std::string &virtualhost_name,
                                                                                             const std::string &begin_date,
                                                                                             const std::string &end_date) {
  auto agent_id = general_database_functions_->GetAgentNameId(agent_name);
  auto virtualhost_id = apache_database_functions_->GetVirtualhostNameId(virtualhost_name);

  ::apache::type::AnomalyDetectionConfigurationEntry c;
  c.agent_name = agent_name;
  c.virtualhost_name = virtualhost_name;
  c.begin_date = ::type::Date::Create(begin_date);
  c.end_date = ::type::Date::Create(end_date);

  auto tbegin = ::type::Timestamp::Create(::type::Time(),
                                          ::type::Date::Create(begin_date));
  auto tend = ::type::Timestamp::Create(::type::Time::Create(23, 59, 59),
                                        ::type::Date::Create(end_date));
  auto count = database_->GetApacheSessionStatisticsCount(agent_name, virtualhost_name,
                                                          tbegin, tend);

  ::apache::type::ApacheSessions sessions = database_->GetApacheSessionStatistics(agent_name, virtualhost_name,
                                                                                  tbegin, tend,
                                                                                  count, 0);

  ::database::type::RowIds sessions_ids;
  for (auto s : sessions)
    sessions_ids.push_back(s.id);

  general_database_functions_->AddDate(c.begin_date);
  general_database_functions_->AddDate(c.end_date);
  database_->SetApacheAnomalyDetectionConfiguration(c);

  apache_database_functions_->RemoveAllLearningSessions(agent_id, virtualhost_id);
  apache_database_functions_->SetLearningSessions(agent_id, virtualhost_id, sessions_ids);

  json j;
  j["status"] = "ok";

  return j.dump();
}

const ::web::type::JsonMessage CommandExecutorObject::GetSessionsWithoutLearningSet(const std::string &agent_name,
                                                                                    const std::string &virtualhost_name,
                                                                                    const std::string &begin_date,
                                                                                    const std::string &end_date) {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::GetSessionsWithoutLearningSet: Function call";

  auto tbegin = ::type::Timestamp::Create(::type::Time(),
                                          ::type::Date::Create(begin_date));
  auto tend = ::type::Timestamp::Create(::type::Time::Create(23, 59, 59),
                                        ::type::Date::Create(end_date));
  auto count = apache_database_functions_->GetSessionStatisticsWithoutLearningSetCount(agent_name, virtualhost_name,
                                                                                       tbegin, tend);

  ::apache::type::ApacheSessions sessions = apache_database_functions_->GetSessionStatisticsWithoutLearningSet(agent_name, virtualhost_name,
                                                                                                               tbegin, tend,
                                                                                                               count, 0);

  json j, r = json::array();
  for (::apache::type::ApacheSessionEntry s : sessions) {
    json t;
    t["id"] = s.id;
    t["agent_name"] = s.agent_name;
    t["virtualhost"] = s.virtualhost;
    t["client_ip"] = s.client_ip;
    t["session_start"] = s.session_start.ToString();
    t["session_length"] = s.session_length;
    t["bandwidth_usage"] = s.bandwidth_usage;
    t["requests_count"] = s.requests_count;
    t["error_percentage"] = s.error_percentage;
    t["useragent"] = s.useragent;
    t["classification"] = static_cast<int>(s.classification);

    r.push_back(t);
  }

  j["status"] = "ok";
  j["result"] = r;

  return j.dump();
}

const ::web::type::JsonMessage CommandExecutorObject::MarkLearningSetWithIqrMethod(const std::string &agent_name,
                                                                                   const std::string &virtualhost_name) {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::MarkLearningSetWithIqrMethod: Function call";

  auto agent_id = general_database_functions_->GetAgentNameId(agent_name);
  auto virtualhost_id = apache_database_functions_->GetVirtualhostNameId(virtualhost_name);

  apache_database_functions_->MarkLearningSetWithIqrMethod(agent_id, virtualhost_id);

  json j;
  j["status"] = "ok";

  return j.dump();
}

const ::web::type::JsonMessage CommandExecutorObject::GetAgentsAndVirtualhostsNamesFilteredBySessionsClassificationExists() {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::GetAgentsAndVirtualhostsNamesFilteredBySessionsClassificationExists: Function call";

  json j, r = json::array();

  auto agent_names = general_database_functions_->GetAgentNames();
  for (auto agent : agent_names) {
    auto t = json::array();

    auto hosts_names = apache_database_functions_->GetVirtualhostNames(agent);
    for (auto host : hosts_names) {
      if (apache_database_functions_->IsSessionStatisticsWithoutLearningSetExists(agent, host))
        t.push_back(host);
    }

    if (t.size() > 0) {
      json m;
      m["agent_name"] = agent;
      m["virtualhosts_names"] = t;
      r.push_back(m);
    }
  }

  j["status"] = "ok";
  j["result"] = r;

  return j.dump();
}

const ::web::type::JsonMessage CommandExecutorObject::GetLearningSetSessions(const std::string &agent_name,
                                                                             const std::string &virtualhost_name) {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::GetLearningSetSessions: Function call";

  auto agent = general_database_functions_->GetAgentNameId(agent_name);
  auto virtualhost = apache_database_functions_->GetVirtualhostNameId(virtualhost_name);

  auto count = apache_database_functions_->GetLearningSessionsCount(agent, virtualhost);
  ::apache::type::ApacheSessions sessions = apache_database_functions_->GetLearningSessions(agent, virtualhost, count, 0);

  json j, r = json::array();
  for (::apache::type::ApacheSessionEntry s : sessions) {
    json t;
    t["id"] = s.id;
    t["agent_name"] = s.agent_name;
    t["virtualhost"] = s.virtualhost;
    t["client_ip"] = s.client_ip;
    t["session_start"] = s.session_start.ToString();
    t["session_length"] = s.session_length;
    t["bandwidth_usage"] = s.bandwidth_usage;
    t["requests_count"] = s.requests_count;
    t["error_percentage"] = s.error_percentage;
    t["useragent"] = s.useragent;
    t["classification"] = static_cast<int>(s.classification);

    r.push_back(t);
  }

  j["status"] = "ok";
  j["result"] = r;

  return j.dump();
}

const ::web::type::JsonMessage CommandExecutorObject::RemoveConfiguration(const std::string &id) {
  BOOST_LOG_TRIVIAL(debug) << "apache::web::CommandExecutorObject::RemoveConfiguration: Function call";

  ::database::type::RowId row_id = std::stoll(id);

  for (auto c : apache_database_functions_->GetAnomalyDetectionConfigurations()) {
    if (c.id == row_id) {
      apache_database_functions_->RemoveAllLearningSessions(general_database_functions_->GetAgentNameId(c.agent_name),
                                                            apache_database_functions_->GetVirtualhostNameId(c.virtualhost_name));
      break;
    }
  }

  apache_database_functions_->RemoveAnomalyDetectionConfiguration(row_id);

  json j;
  j["status"] = "ok";

  return j.dump();
}

}

}
