#include <iostream>
#include <string>

#include "gflags/gflags.h"

#include "trpc/client/make_client_context.h"
#include "trpc/client/trpc_client.h"
#include "trpc/common/runtime_manager.h"
#include "trpc/log/trpc_log.h"
#include "trpc/util/http/http_handler.h"

#include "issueimpl.trpc.pb.h"

DEFINE_string(client_config, "trpc_cpp.yaml", "framework client config file, --client_config=trpc_cpp.yaml");
DEFINE_string(service_name, "trpc.test.issueimpl.Introducer", "callee service name");


int IntroducerSelfIntroduction(const std::shared_ptr<::trpc::test::issueimpl::IntroducerServiceProxy>& proxy) {
  ::trpc::ClientContextPtr client_ctx = ::trpc::MakeClientContext(proxy);

  rapidjson::Document req_json;
  req_json.Parse(R"({ "name": "issueshooter", "age": 18, "hobby": ["opensource project","movies", "books"]})");
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  req_json.Accept(writer);
  ::trpc::test::issueimpl::IntroduceRequest req;
  req.set_msg(buffer.GetString());
  // fill some filed of req
  ::trpc::test::issueimpl::IntroduceReply rsp;
  ::trpc::Status status = proxy->SelfIntroduction(client_ctx, req, &rsp);
  if (!status.OK()) {
    std::cerr << "get rpc error: " << status.ErrorMessage() << std::endl;
    return -1;
  }
  std::cout << "get rsp success" << std::endl;
  std::cout << rsp.msg() << std::endl;
  return 0;
}

int Run() {
  auto proxy = ::trpc::GetTrpcClient()->GetProxy<::trpc::test::issueimpl::IntroducerServiceProxy>(FLAGS_service_name);
  return IntroducerSelfIntroduction(proxy);
}

void ParseClientConfig(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::CommandLineFlagInfo info;
  if (GetCommandLineFlagInfo("client_config", &info) && info.is_default) {
    TRPC_FMT_ERROR("start client with config, for example:{} --client_config=/client/config/filepath", argv[0]);
    exit(-1);
  }
  
  std::cout << "FLAGS_service_name: " << FLAGS_service_name << std::endl;
  std::cout << "FLAGS_client_config: " << FLAGS_client_config << std::endl;

  int ret = ::trpc::TrpcConfig::GetInstance()->Init(FLAGS_client_config);
  if (ret != 0) {
    std::cerr << "load client_config failed." << std::endl;
    exit(-1);
  }
}

int main(int argc, char* argv[]) {
  ParseClientConfig(argc, argv);
  // If the business code is running in trpc pure client mode,
  // the business code needs to be running in the `RunInTrpcRuntime` function
  // This function can be seen as a program entry point and should be called only once.
  return ::trpc::RunInTrpcRuntime([]() {
    return Run();
  });
}
