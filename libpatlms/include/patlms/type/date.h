#ifndef INCLUDE_PATLMS_TYPE_DATE_H
#define INCLUDE_PATLMS_TYPE_DATE_H

#include <ostream>
#include <string>

namespace type
{

class Date {
 public:
  inline Date();
  Date(const Date&) = default;

  static const Date Create(int day, int month, int year);
  static const Date Create(const std::string &time);

  void Set(int day, int month, int year);

  const std::string ToString() const;

  inline int GetDay() const;
  inline int GetMonth() const;
  inline int GetYear() const;

 private:
  static void CheckDate(int day, int month, int year);

  int day_, month_, year_;
};

Date::Date() :
day_(0),
month_(0),
year_(0) {
}

int Date::GetDay() const {
  return day_;
}

int Date::GetMonth() const {
  return month_;
}

int Date::GetYear() const {
  return year_;
}

std::ostream& operator<<(std::ostream& os, const type::Date &date);

}

#endif /* INCLUDE_PATLMS_TYPE_DATE_H */