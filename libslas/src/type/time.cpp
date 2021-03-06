#include <slas/type/time.h>

#include <slas/type/exception/detail/wrong_time_value_exception.h>
#include <slas/type/exception/detail/time_parse_exception.h>

#include <regex>
#include <iostream>
namespace type
{

const Time Time::Create(int hour, int minute, int second) {
  Time t;
  t.Set(hour, minute, second);

  return t;
}

const Time Time::Create(const std::string &time) {
  int hour = 0, minute = 0, second = 0;
  std::regex pattern("^\\s*([0-9]{1,2}):([0-9]{1,2}):([0-9]{1,2})\\s*$");

  std::cmatch cm;
  if (std::regex_match(time.c_str(), cm, pattern)) {
    hour = stoi(cm[1]);
    minute = stoi(cm[2]);
    second = stoi(cm[3]);
  }
  else
    throw exception::detail::TimeParseException();

  return Time::Create(hour, minute, second);
}

void Time::Set(int hour, int minute, int second) {
  CheckTime(hour, minute, second);

  hour_ = hour;
  minute_ = minute;
  second_ = second;
}

const std::string Time::ToString() const {
  auto helper = [](int i) {
    return (i < 10 ? "0" : "") +std::to_string(i);
  };

  const std::string time_string = helper(hour_) + ":" + helper(minute_) + ":" + helper(second_);

  return time_string;
}

bool Time::operator==(const Time &t2) const {
  return (GetHour() == t2.GetHour()) &&
      (GetMinute() == t2.GetMinute()) &&
      (GetSecond() == t2.GetSecond());
}

bool Time::operator<(const Time &t2) const {
  if ((GetHour() == t2.GetHour()) &&
      (GetMinute() == t2.GetMinute()) &&
      (GetSecond() < t2.GetSecond()))
    return true;
  else if ((GetHour() == t2.GetHour()) &&
      (GetMinute() < t2.GetMinute()))
    return true;
  else if ((GetHour() < t2.GetHour()))
    return true;

  return false;
}

Time Time::operator-(long seconds) const {
  int hour = 0, minute = 0, second = 0;
  long time_in_seconds = GetHour() * 60 * 60 + GetMinute() * 60 + GetSecond() - seconds;

  if (time_in_seconds > 0) {
    hour = time_in_seconds / (60 * 60);
    time_in_seconds = time_in_seconds - hour * (60 * 60);

    minute = time_in_seconds / 60;
    time_in_seconds = time_in_seconds - minute * 60;

    second = time_in_seconds;
  }

  return Time::Create(hour, minute, second);
}

Time Time::operator+(long seconds) const {
  int hour = 23, minute = 59, second = 59;

  if (seconds < 23 * 60 * 60 + 59 * 60 + 59) {
    long time_in_seconds = GetHour() * 60 * 60 + GetMinute() * 60 + GetSecond() + seconds;

    if (time_in_seconds < 23 * 60 * 60 + 59 * 60 + 59) {
      hour = time_in_seconds / (60 * 60);
      time_in_seconds = time_in_seconds - hour * (60 * 60);

      minute = time_in_seconds / 60;
      time_in_seconds = time_in_seconds - minute * 60;

      second = time_in_seconds;
    }
  }

  return Time::Create(hour, minute, second);
}

void Time::CheckTime(int hour, int minute, int second) {
  if ((hour < 0 || hour > 23) ||
      (minute < 0 || minute > 59) ||
      (second < 0 || second > 59)) {
    throw exception::detail::WrongTimeValueException();
  }
}

std::ostream& operator<<(std::ostream& os, const type::Time &time) {
  os << time.GetHour() << ":" << time.GetMinute() << ":" << time.GetSecond();
  return os;
}

}
