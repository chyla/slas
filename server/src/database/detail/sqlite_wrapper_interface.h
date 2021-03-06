/*
 * Copyright 2016 Adam Chyła, adam@chyla.org
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#pragma once

#include <memory>

#include "sqlite_interface.h"

namespace database
{

namespace detail
{

class SQLiteWrapperInterface {
 public:
  virtual ~SQLiteWrapperInterface() = default;

  virtual void Open(const std::string &file_path) = 0;
  virtual bool IsOpen() const = 0;
  virtual bool Close() = 0;

  virtual void Prepare(const std::string &sql, sqlite3_stmt **ppStmt) = 0;

  virtual void BindDouble(sqlite3_stmt *pStmt, int pos, double value) = 0;
  virtual void BindInt(sqlite3_stmt *pStmt, int pos, int value) = 0;
  virtual void BindInt64(sqlite3_stmt *pStmt, int pos, sqlite3_int64 value) = 0;
  virtual void BindText(sqlite3_stmt *pStmt, int pos, const std::string &value) = 0;

  virtual double ColumnDouble(sqlite3_stmt *pStmt, int iCol) = 0;
  virtual int ColumnInt(sqlite3_stmt *pStmt, int iCol) = 0;
  virtual sqlite3_int64 ColumnInt64(sqlite3_stmt *pStmt, int iCol) = 0;
  virtual const std::string ColumnText(sqlite3_stmt *pStmt, int iCol) = 0;

  virtual int Step(sqlite3_stmt *pStmt) = 0;
  virtual void Finalize(sqlite3_stmt *pStmt) = 0;

  virtual void Exec(const std::string &sql, int (*callback) (void *, int, char **, char **) = nullptr, void *arg = nullptr) = 0;
  virtual long long GetFirstInt64Column(const std::string &sql) = 0;
  virtual long long GetFirstInt64Column(const std::string &sql, long long default_return_value) = 0;

  virtual sqlite3* GetSQLiteHandle() = 0;
};

typedef std::shared_ptr<SQLiteWrapperInterface> SQLiteWrapperInterfacePtr;

}

}
