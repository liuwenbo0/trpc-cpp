#include "server/server.h"

#include <memory>
#include <string>

#include "fmt/format.h"

#include "trpc/log/trpc_log.h"

#include "service.h"

namespace trpc {
namespace test {
namespace issueimpl {

// The initialization logic depending on framework runtime(threadmodel,transport,etc) or plugins(config,metrics,etc) should be placed here.
// Others can simply place at main function(before Main() function invoked)
int IssueimplServer::Initialize() {
  const auto& config = ::trpc::TrpcConfig::GetInstance()->GetServerConfig();

  // Set the service name, which must be the same as the value of the `/server/service/name` configuration item in the yaml file,
  // otherwise the framework cannot receive requests normally.
  std::string service_name = fmt::format("{}.{}.{}.{}", "trpc", config.app, config.server, "Introducer");
  ::trpc::ServicePtr my_service(std::make_shared<IntroducerServiceImpl>());
  RegisterService(service_name, my_service);
  TRPC_FMT_INFO("Register service: {}", service_name);
  
  return 0;
}

// If the resources initialized in the initialize function need to be destructed when the program exits, it is recommended to place them here.
void IssueimplServer::Destroy() {}

}  // namespace issueimpl
}  // namespace test
}  // namespace trpc

int main(int argc, char** argv) {
  ::trpc::test::issueimpl::IssueimplServer issueimpl_server;
  issueimpl_server.Main(argc, argv);
  issueimpl_server.Wait();

  return 0;
}
