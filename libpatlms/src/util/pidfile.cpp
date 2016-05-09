#include <patlms/util/pidfile.h>
#include <patlms/util/detail/cant_create_pidfile_exception.h>
#include <patlms/util/detail/cant_remove_pidfile_exception.h>

#include <string>
#include <cstring>
#include <cerrno>
#include <boost/log/trivial.hpp>

using namespace std;

namespace util
{

void CreatePidFile(const std::string &pidfile_path, detail::SystemInterfacePtr system) {
  bool failed = false;
  BOOST_LOG_TRIVIAL(debug) << "libpatlms::util::CreatePidFile: Function call with (pidfile_path=" << pidfile_path << ")";

  if (system->access(pidfile_path.c_str(), F_OK) != -1) {
    BOOST_LOG_TRIVIAL(error) << "libpatlms::util::CreatePidFile: File exists: " << strerror(errno);
    throw detail::CantCreatePidfileException();
  }

  FILE *f = system->fopen(pidfile_path.c_str(), "w");
  if (f == NULL) {
    failed = true;
    BOOST_LOG_TRIVIAL(error) << "libpatlms::util::CreatePidFile: Couldn't create pid file: " << strerror(errno);
  }
  else {
    int ret;
    string pid_str = to_string(system->getpid());

    BOOST_LOG_TRIVIAL(debug) << "libpatlms::util::CreatePidFile: Writing pid: " << pid_str;
    ret = system->fwrite(pid_str.c_str(), pid_str.length(), 1, f);
    if (ret != pid_str.length()) {
      failed = true;
      BOOST_LOG_TRIVIAL(error) << "libpatlms::util::CreatePidFile: Written count: " << ret;
      BOOST_LOG_TRIVIAL(error) << "libpatlms::util::CreatePidFile: Failed to write pid: " << strerror(errno);
    }

    BOOST_LOG_TRIVIAL(debug) << "libpatlms::util::CreatePidFile: Closing pid file...";
    ret = system->fclose(f);
    if (ret < 0) {
      failed = true;
      BOOST_LOG_TRIVIAL(error) << "libpatlms::util::CreatePidFile: Failed to close file: " << strerror(errno);
    }
  }

  if (failed) {
    throw detail::CantCreatePidfileException();
  }

  BOOST_LOG_TRIVIAL(debug) << "libpatlms::util::CreatePidFile: Done";
}

void RemovePidFile(const std::string &pidfile_path, detail::SystemInterfacePtr system) {
  BOOST_LOG_TRIVIAL(debug) << "libpatlms::util::RemovePidFile: Function call with (pidfile_path=" << pidfile_path << ")";

  int ret = system->unlink(pidfile_path.c_str());
  if (ret < 0) {
    BOOST_LOG_TRIVIAL(error) << "libpatlms::util::RemovePidFile: Couldn't remove pid file: " << strerror(errno);
    throw detail::CantRemovePidfileException();
  }

  BOOST_LOG_TRIVIAL(debug) << "libpatlms::util::RemovePidFile: Done";
}

}