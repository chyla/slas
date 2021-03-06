#include "bash.h"
#include "src/bash/domain/scripts.h"

#include <boost/log/trivial.hpp>

namespace bash
{

namespace dbus
{

namespace object
{

Bash::Bash(::bash::domain::detail::ScriptsInterfacePtr scripts) :
scripts_(scripts) {
}

Bash::~Bash() {
}

const char* Bash::GetPath() {
  return "/org/chyla/slas/bash";
}

const char* Bash::GetXmlInterface() {
  const char *xml =
      "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
      "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
      "<node>\n"
      "  <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
      "    <method name=\"Introspect\">\n"
      "      <arg name=\"data\" direction=\"out\" type=\"s\"/>\n"
      "    </method>\n"
      "  </interface>\n"
      "  <interface name=\"org.chyla.slas.bash\">\n"
      "    <method name=\"AddLogEntry\">\n"
      "      <arg direction=\"in\" type=\"s\"/>\n"
      "      <arg direction=\"in\" type=\"i\"/>\n"
      "      <arg direction=\"in\" type=\"i\"/>\n"
      "      <arg direction=\"in\" type=\"i\"/>\n"
      "      <arg direction=\"in\" type=\"i\"/>\n"
      "      <arg direction=\"in\" type=\"i\"/>\n"
      "      <arg direction=\"in\" type=\"i\"/>\n"
      "      <arg direction=\"in\" type=\"u\"/>\n"
      "      <arg direction=\"in\" type=\"s\"/>\n"
      "      <arg direction=\"out\" type=\"v\"/>\n"
      "    </method>\n"
      "  </interface>\n"
      "</node>\n";

  return xml;
}

DBusHandlerResult Bash::OwnMessageHandler(DBusConnection *connection, DBusMessage *message) {
  BOOST_LOG_TRIVIAL(debug) << "objects::Bash::OwnMessageHandler: Function call";

  if (dbus_message_is_method_call(message, "org.chyla.slas.bash", "AddLogEntry")) {
    BOOST_LOG_TRIVIAL(debug) << "objects:Bash:OwnMessageHandler: Received method call org.chyla.slas.bash.AddLogEntry";

    const char* agent_name = nullptr;
    int hour, minute, second, day, month, year;
    unsigned user_id;
    const char* command = nullptr;
    dbus_message_get_args(message, NULL,
                          DBUS_TYPE_STRING, &agent_name,
                          DBUS_TYPE_INT32, &hour,
                          DBUS_TYPE_INT32, &minute,
                          DBUS_TYPE_INT32, &second,
                          DBUS_TYPE_INT32, &day,
                          DBUS_TYPE_INT32, &month,
                          DBUS_TYPE_INT32, &year,
                          DBUS_TYPE_UINT32, &user_id,
                          DBUS_TYPE_STRING, &command,
                          DBUS_TYPE_INVALID);

    BOOST_LOG_TRIVIAL(debug) << "objects::Bash::OwnMessageHandler: Add log entry to cache: "
        << "agent_name=" << agent_name << " ; "
        << "hour=" << hour << " ; "
        << "minute=" << minute << " ; "
        << "second=" << second << " ; "
        << "day=" << day << " ; "
        << "month=" << month << " ; "
        << "year=" << year << " ; "
        << "user_id=" << user_id << " ; "
        << "command=" << command;

    type::BashLogEntry log_entry;
    log_entry.agent_name = agent_name;
    log_entry.utc_time.Set(hour, minute, second, day, month, year);
    log_entry.user_id = user_id;
    log_entry.command = command;

    scripts_->AddLog(log_entry);

    DBusMessage *reply_msg = dbus_message_new_method_return(message);
    BOOST_LOG_TRIVIAL(debug) << "objects::Bash::OwnMessageHandler: Sending reply";
    dbus_connection_send(connection, reply_msg, NULL);

    BOOST_LOG_TRIVIAL(debug) << "objects::Bash::OwnMessageHandler: Connection flushing";
    dbus_connection_flush(connection);

    BOOST_LOG_TRIVIAL(debug) << "objects::Bash::OwnMessageHandler: Done. Returning DBUS_HANDLER_RESULT_HANDLED";

    return DBUS_HANDLER_RESULT_HANDLED;
  }

  BOOST_LOG_TRIVIAL(warning) << "objects::Bash::OwnMessageHandler: Possible bug: DBUS_HANDLER_RESULT_NOT_YET_HANDLED";

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

}

}

}
