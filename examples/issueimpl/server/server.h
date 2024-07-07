#pragma once

#include "trpc/common/trpc_app.h"

namespace trpc {
namespace test {
namespace issueimpl {

class IssueimplServer : public ::trpc::TrpcApp {
 public:
  int Initialize() override;

  void Destroy() override;
};

}  // namespace issueimpl
}  // namespace test
}  // namespace trpc
