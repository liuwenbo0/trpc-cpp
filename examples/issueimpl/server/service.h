#pragma once

#include "issueimpl.trpc.pb.h"

namespace trpc {
namespace test {
namespace issueimpl {

class IntroducerServiceImpl : public ::trpc::test::issueimpl::Introducer {
 public:
  ::trpc::Status SelfIntroduction(::trpc::ServerContextPtr context, const ::trpc::test::issueimpl::IntroduceRequest* request, ::trpc::test::issueimpl::IntroduceReply* reply) override;
};

}  // namespace issueimpl
}  // namespace test
}  // namespace trpc
