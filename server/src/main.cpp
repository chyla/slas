#include <iostream>
#include <boost/log/trivial.hpp>
#include <memory>
#include <signal.h>
#include <iostream>
#include <thread>

#include <slas/dbus/bus.h>
#include <slas/type/exception/exception.h>
#include <slas/util/demonize.h>
#include <slas/util/configure_logger.h>
#include <slas/util/create_pidfile.h>
#include <slas/util/remove_file.h>

#include "program_options/parser.h"
#include "database/database.h"
#include "database/sqlite_wrapper.h"
#include "database/general_database_functions.h"
#include "web/command_receiver.h"
#include "web/command_executor.h"
#include "program_options/web/command_executor_object.h"

#include "apache/web/command_executor_object.h"
#include "src/apache/dbus/object/apache.h"
#include "src/bash/dbus/object/bash.h"

#include "analyzer/analyzer.h"
#include "apache/analyzer/apache_analyzer_object.h"

#include "src/bash/database/database_functions.h"
#include "src/bash/domain/scripts.h"
#include "src/bash/domain/web_scripts.h"
#include "src/bash/web/command_executor_object.h"

#include "notifier/notifier.h"
#include "bash/analyzer/bash_analyzer_object.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

dbus::BusPtr bus;
web::CommandReceiverPtr command_receiver;
analyzer::AnalyzerPtr analyzer_worker;
notifier::NotifierPtr notifier_worker;
std::thread command_receiver_thread;
std::thread analyzer_thread;
std::thread notifier_thread;

struct sigaction act, act_usr;

void exit_signal_handler(int sig, siginfo_t *siginfo, void *context) {
  bus->StopLoop();
  command_receiver->StopListen();
  analyzer_worker->StopLoop();
  notifier_worker->StopLoop();
}

void analyze_signal_handler(int sig, siginfo_t *siginfo, void *context) {
  analyzer_worker->StartAnalyzing();
}

database::DatabasePtr CreateDatabase(database::SQLiteWrapperPtr sqlite_wrapper) {
  database::DatabasePtr database = database::Database::Create();
  database->Open(sqlite_wrapper->GetSQLiteHandle());

  return database;
}

int
main(int argc, char *argv[]) {
  program_options::Parser p;
  program_options::type::Options options;
  bash::dbus::object::BashPtr bash_object;
  apache::dbus::object::ApachePtr apache_object;
  database::SQLiteWrapperPtr sqlite_wrapper;
  database::DatabasePtr database;
  database::GeneralDatabaseFunctionsPtr general_database_functions;
  apache::database::DatabaseFunctionsPtr apache_database_functions;

  try {
    p.SetCommandLineOptions(argc, argv);
    p.SetConfigFilePath(SYSCONFFILE);
    options = p.Parse();

    if (options.IsShowHelpMessage()) {
      std::cout << "Usage: " << argv[0] << " " << "[options]\n";
      std::cout << p.GetHelpMessage() << '\n';
      return 0;
    }
  }
  catch (interface::Exception &ex) {
    std::cerr << "Error: Wrong option: " << ex.what() << '\n';
    return 1;
  }
  catch (std::exception &ex) {
    std::cerr << "Error: Missing or wrong program option.\n";
    std::cerr << "Error detail: " << ex.what() << "\n\n";
    std::cout << "Usage: " << argv[0] << " " << "[options]\n";
    std::cerr << p.GetHelpMessage() << "\n";
    return 1;
  }

  try {
    util::ConfigureLogger(options.GetLogfilePath(), options.IsDebug());
  }
  catch (std::exception &ex) {
    std::cerr << "Error: Can't configure logger: " << ex.what() << '\n';
    return 1;
  }

  try {
    BOOST_LOG_TRIVIAL(info) << "SLAS Server";
#ifdef HAVE_CONFIG_H
    BOOST_LOG_TRIVIAL(info) << "Version: " << VERSION;
#endif

    util::Demonize(options.IsDaemon());

    sqlite_wrapper = database::SQLiteWrapper::Create();
    sqlite_wrapper->Open(options.GetDatabasefilePath());
    database = CreateDatabase(sqlite_wrapper);
    general_database_functions = database::GeneralDatabaseFunctions::Create(database,
                                                                            sqlite_wrapper);
    general_database_functions->CreateTables();
    apache_database_functions = apache::database::DatabaseFunctions::Create(database,
                                                                            sqlite_wrapper,
                                                                            general_database_functions);
    apache_database_functions->CreateTables();

    auto options_command_object = program_options::web::CommandExecutorObject::Create(options);
    auto command_executor = web::CommandExecutor::Create();
    auto apache_web_command_executor = apache::web::CommandExecutorObject::Create(database,
                                                                                  general_database_functions,
                                                                                  apache_database_functions);
    command_executor->RegisterCommandObject(options_command_object);
    command_executor->RegisterCommandObject(apache_web_command_executor);
    command_receiver = web::CommandReceiver::Create(command_executor);
    command_receiver->OpenPort(options.GetWebAddress(), options.GetWebPort());

    bus = std::make_shared<dbus::Bus>(dbus::Bus::Options(options.GetDbusAddress(),
                                                         options.GetDbusPort(),
                                                         options.GetDbusFamily()));
    bus->Connect();
    bus->RequestConnectionName("org.chyla.slas.server");

    auto bash_database_functions = ::bash::database::DatabaseFunctions::Create(sqlite_wrapper,
                                                                               general_database_functions);
    bash_database_functions->CreateTables();

    auto bash_scripts = ::bash::domain::Scripts::Create(bash_database_functions,
                                                        general_database_functions);

    auto bash_web_scripts = ::bash::domain::WebScripts::Create(bash_scripts);

    auto bash_web_command_executor = bash::web::CommandExecutorObject::Create(bash_web_scripts);
    command_executor->RegisterCommandObject(bash_web_command_executor);

    bash_object = std::make_shared<bash::dbus::object::Bash>(bash_scripts);
    bus->RegisterObject(bash_object);

    apache_object = std::make_shared<apache::dbus::object::Apache>(database, general_database_functions, apache_database_functions);
    bus->RegisterObject(apache_object);

    util::CreatePidFile(options.GetPidfilePath());

    memset(&act, '\0', sizeof (act));
    act.sa_sigaction = exit_signal_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGKILL, &act, nullptr);
    sigaction(SIGQUIT, &act, nullptr);

    memset(&act, '\0', sizeof (act_usr));
    act_usr.sa_sigaction = analyze_signal_handler;
    act_usr.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act_usr, nullptr);

    notifier_worker = notifier::Notifier::Create(options);
    notifier_thread = std::thread([]() {
      notifier_worker->Loop();
    });

    analyzer_worker = analyzer::Analyzer::Create();
    analyzer_worker->AddObject(apache::analyzer::ApacheAnalyzerObject::Create(general_database_functions,
                                                                              apache_database_functions,
                                                                              notifier_worker));

    analyzer_worker->AddObject(bash::analyzer::BashAnalyzerObject::Create(bash_database_functions,
                                                                          general_database_functions,
                                                                          bash_scripts,
                                                                          options.GetNeuralNetworkDataDirectory()));

    analyzer_thread = std::thread([]() {
      analyzer_worker->StartLoop();
    });

    command_receiver_thread = std::thread([]() {
      command_receiver->StartListen();
    });

    bus->Loop();
  }
  catch (std::exception &ex) {
    BOOST_LOG_TRIVIAL(fatal) << ex.what();
    return 1;
  }

  try {
    if (analyzer_worker)
      analyzer_worker->StopLoop();

    if (notifier_worker)
      notifier_worker->StopLoop();

    if (command_receiver)
      command_receiver->StopListen();

    if (bus)
      bus->Disconnect();

    if (sqlite_wrapper)
      sqlite_wrapper->Close();

    if (analyzer_thread.joinable())
      analyzer_thread.join();

    if (notifier_thread.joinable())
      notifier_thread.join();

    if (command_receiver_thread.joinable())
      command_receiver_thread.join();

    if (command_receiver)
      command_receiver->ClosePort();

    util::RemoveFile(options.GetPidfilePath());
  }
  catch (std::exception &ex) {
    BOOST_LOG_TRIVIAL(fatal) << ex.what();
    return 1;
  }

  return 0;
}

