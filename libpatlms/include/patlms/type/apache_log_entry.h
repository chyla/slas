#ifndef APACHE_LOG_ENTRY_H
#define APACHE_LOG_ENTRY_H

#include <patlms/type/time.h>

namespace type
{

struct ApacheLogEntry {
  std::string virtualhost;
  std::string client_ip;
  Time time;
  std::string request;
  int status_code;
  int bytes;
  std::string user_agent;
};

}

#endif /* APACHE_LOG_ENTRY_H */
