#ifndef PATLMS_DBUS_DBUS_THREAD_H
#define PATLMS_DBUS_DBUS_THREAD_H

#include <patlms/dbus/detail/bus_interface.h>
#include <memory>
#include <list>
#include <mutex>

#include "dbus_thread_command.h"
#include "detail/system.h"
#include "detail/system_interface.h"

namespace dbus
{

class DBusThread {
 public:
  typedef std::shared_ptr<DBusThreadCommand> CommandPtr;
  typedef std::shared_ptr<DBusThread> DBusThreadPtr;
  typedef std::shared_ptr<dbus::detail::BusInterface> BusInterfacePtr;
  typedef std::shared_ptr<detail::SystemInterface> SystemInterfacePtr;

  static DBusThreadPtr Create(BusInterfacePtr bus);
  static DBusThreadPtr Create(BusInterfacePtr bus, SystemInterfacePtr system);

  void AddCommand(CommandPtr command);

  void StartLoop();
  void StopLoop();

  bool IsLoopRunning();

 private:
  typedef std::list<CommandPtr> ThreadCommands;

  DBusThread(BusInterfacePtr bus, SystemInterfacePtr system);

  CommandPtr GetCommand();

  BusInterfacePtr bus_;
  SystemInterfacePtr system_;

  ThreadCommands commands_;
  std::mutex commands_mutex_;
  bool loop_running_;
};

}

#endif /* PATLMS_DBUS_DBUS_THREAD_H */
