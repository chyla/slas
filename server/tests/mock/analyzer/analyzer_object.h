#pragma once

#include <memory>
#include <gmock/gmock.h>

#include "src/analyzer/analyzer_object_interface.h"

namespace mock
{

namespace analyzer
{

class AnalyzerObject;
typedef std::shared_ptr<AnalyzerObject> AnalyzerObjectPtr;

class AnalyzerObject : public ::analyzer::AnalyzerObjectInterface {
 public:
  virtual ~AnalyzerObject() = default;

  static AnalyzerObjectPtr Create() {
    return std::make_shared<AnalyzerObject>();
  }

  MOCK_METHOD0(Analyze, void());
};

}

}
