#ifndef APACHE_DBUS_THREAD_COMMAND_H
#define APACHE_DBUS_THREAD_COMMAND_H

#include "apache_proxy.h"

#include "src/dbus/dbus_thread_command.h"

#include <string>

#include <patlms/type/bash_log_entry.h>

namespace apache
{

namespace detail
{

class ApacheDBusThreadCommand : public ::dbus::DBusThreadCommand {
 public:
  ApacheDBusThreadCommand(const type::ApacheLogEntry log_entry,
                          std::shared_ptr<ApacheProxy> bash_proxy);
  virtual ~ApacheDBusThreadCommand();

  void Execute() override;

 private:
  const type::ApacheLogEntry log_entry_;
  std::shared_ptr<ApacheProxy> proxy_;
};


}

}


#endif /* APACHE_DBUS_THREAD_COMMAND_H */