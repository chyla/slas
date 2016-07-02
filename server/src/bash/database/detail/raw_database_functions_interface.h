/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#ifndef RAW_DATABASE_FUNCTIONS_INTERFACE_H
#define RAW_DATABASE_FUNCTIONS_INTERFACE_H

#include "entity/log.h"
#include "entity/system_user.h"

#include <memory>

namespace bash
{

namespace database
{

namespace detail
{

class RawDatabaseFunctionsInterface {
 public:
  virtual ~RawDatabaseFunctionsInterface() = default;

  virtual void CreateTables() = 0;

  virtual void AddSystemUser(const entity::SystemUser &system_user) = 0;
  virtual ::database::type::RowId GetSystemUserId(const entity::SystemUser &system_user) = 0;

  virtual void AddLog(const entity::Log &log) = 0;
};

typedef std::shared_ptr<RawDatabaseFunctionsInterface> RawDatabaseFunctionsInterfacePtr;

}

}

}

#endif /* RAW_DATABASE_FUNCTIONS_INTERFACE_H */
