#ifndef SRC_BASH_DBUS_OBJECT_BASH_H
#define SRC_BASH_DBUS_OBJECT_BASH_H

#include <vector>
#include <memory>

#include <patlms/dbus/object.h>
#include <patlms/type/bash_log_entry.h>

#include "src/bash/domain/detail/scripts_interface.h"

namespace bash
{

namespace dbus
{

namespace object
{

class Bash : public ::dbus::Object {
 public:
  Bash(::bash::domain::detail::ScriptsInterfacePtr scripts);
  virtual ~Bash();

  const char* GetPath();

 protected:
  const char* GetXmlInterface();

  DBusHandlerResult OwnMessageHandler(DBusConnection *connection, DBusMessage *message);

  ::bash::domain::detail::ScriptsInterfacePtr scripts_;
};

typedef std::shared_ptr<Bash> BashPtr;

}

}

}

#endif // SRC_BASH_DBUS_OBJECT_BASH_H
