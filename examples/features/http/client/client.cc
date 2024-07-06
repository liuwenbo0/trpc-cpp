//
//
// Tencent is pleased to support the open source community by making tRPC available.
//
// Copyright (C) 2023 THL A29 Limited, a Tencent company.
// All rights reserved.
//
// If you have downloaded a copy of the tRPC source code from Tencent,
// please note that tRPC source code is licensed under the  Apache 2.0 License,
// A copy of the Apache 2.0 License is included in this file.
//
//

#include <iostream>
#include <string>
#include <vector>

#include "gflags/gflags.h"

#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "trpc/client/http/http_service_proxy.h"
#include "trpc/client/make_client_context.h"
#include "trpc/client/trpc_client.h"
#include "trpc/common/config/trpc_config.h"
#include "trpc/common/runtime_manager.h"
#include "trpc/common/status.h"
#include "trpc/common/trpc_plugin.h"
#include "trpc/coroutine/fiber.h"
#include "trpc/coroutine/fiber_latch.h"
#include "trpc/util/log/logging.h"

DEFINE_string(service_name, "http_client", "callee service name");
DEFINE_string(client_config, "trpc_cpp_fiber.yaml", "");
DEFINE_string(addr, "127.0.0.1:24856", "ip:port");

namespace http::demo {

constexpr std::string_view kGreetings{"hello world!"};

using HttpServiceProxyPtr = std::shared_ptr<::trpc::http::HttpServiceProxy>;

namespace {
//// HTTP GET.

bool GetString(const HttpServiceProxyPtr& proxy) {
  auto ctx = ::trpc::MakeClientContext(proxy);
  std::string rsp_str;
  auto status = proxy->GetString(ctx, "http://example.com/issueshoot-test", &rsp_str);
  if (!status.OK()) {
    TRPC_FMT_ERROR("status: {}", status.ToString());
    return false;
  }
  TRPC_FMT_INFO("response content: {}", rsp_str);
  if (false) return false;
  return true;
}

bool GetJson(const HttpServiceProxyPtr& proxy) {
  auto ctx = ::trpc::MakeClientContext(proxy);
  rapidjson::Document rsp_json;
  auto status = proxy->Get(ctx, "http://example.com/hello", &rsp_json);
  if (!status.OK()) {
    TRPC_FMT_ERROR("status: {}", status.ToString());
    return false;
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  rsp_json.Accept(writer);
  auto rsp_str = buffer.GetString();
  TRPC_FMT_INFO("response content: {}", rsp_str);
  if (rsp_json["msg"].GetString() == kGreetings && rsp_json["age"].GetInt() == 18) {
    return true;
  }
  return false;
}

bool GetHttpResponse(const HttpServiceProxyPtr& proxy) {
  auto ctx = ::trpc::MakeClientContext(proxy);
  ::trpc::http::HttpResponse http_rsp;
  auto status = proxy->Get2(ctx, "http://example.com/hello", &http_rsp);
  if (!status.OK()) {
    TRPC_FMT_ERROR("status: {}", status.ToString());
    return false;
  }
  TRPC_FMT_INFO("response content: {}", http_rsp.GetContent());
  const std::string rsp_json = R"({"msg": "hello world!", "age": 18, "height": 180})";
  if (http_rsp.GetContent() != rsp_json) return false;
  return true;
}

bool GetHttpResponseNotFound(const HttpServiceProxyPtr& proxy) {
  auto ctx = ::trpc::MakeClientContext(proxy);
  ::trpc::http::HttpResponse http_rsp;
  auto status = proxy->Get2(ctx, "http://example.com/foo-not-found", &http_rsp);
  if (!status.OK() && status.GetFuncRetCode() == 404) {
    TRPC_FMT_ERROR("status: {}", status.ToString());
    return true;
  }
  return false;
}
}  // namespace

namespace {
//// HTTP HEAD.

bool HeadHttpResponse(const HttpServiceProxyPtr& proxy) {
  auto ctx = ::trpc::MakeClientContext(proxy);
  ::trpc::http::HttpResponse http_rsp;
  auto status = proxy->Head(ctx, "http://example.com/foo", &http_rsp);
  if (!status.OK()) {
    TRPC_FMT_ERROR("status: {}", status.ToString());
    return false;
  }
  TRPC_FMT_INFO("response content: {}", http_rsp.GetContent());
  if (!http_rsp.GetContent().empty()) return false;
  if (http_rsp.GetHeader("Content-Length") != std::to_string(kGreetings.size())) return false;
  return true;
}
}  // namespace

namespace {
//// HTTP POST.

bool PostString(const HttpServiceProxyPtr& proxy) {
  auto ctx = ::trpc::MakeClientContext(proxy);
  std::string req_str{kGreetings};
  std::string rsp_str;
  auto status = proxy->Post(ctx, "http://example.com/foo", std::move(req_str), &rsp_str);
  if (!status.OK()) {
    TRPC_FMT_ERROR("status: {}", status.ToString());
    return false;
  }
  TRPC_FMT_INFO("response content: {}", rsp_str);
  if (rsp_str != kGreetings) return false;
  return true;
}

bool PostJson(const HttpServiceProxyPtr& proxy) {
  auto ctx = ::trpc::MakeClientContext(proxy);
  rapidjson::Document req_json;
  req_json.Parse(R"({ "name": "issueshooter", "age": 18, "hobby": ["opensource project","movies", "books"]})");
  rapidjson::Document rsp_json;
  auto status = proxy->Post(ctx, "http://example.com/issueshoot-test", req_json, &rsp_json);
  if (!status.OK()) {
    TRPC_FMT_ERROR("status: {}", status.ToString());
    return false;
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  rsp_json.Accept(writer);
  auto rsp_str = buffer.GetString();
  TRPC_FMT_INFO("response content: {}", rsp_str);
  if (req_json != rsp_json) {
    return false;
  }
  return true;
}

bool PostStringAndWaitHttpResponse(const HttpServiceProxyPtr& proxy) {
  auto ctx = ::trpc::MakeClientContext(proxy);
  std::string req_str{kGreetings};
  ::trpc::http::HttpResponse http_rsp;
  auto status = proxy->Post2(ctx, "http://example.com/foo", std::move(req_str), &http_rsp);
  if (!status.OK()) {
    TRPC_FMT_ERROR("status: {}", status.ToString());
    return false;
  }
  TRPC_FMT_INFO("response content: {}", http_rsp.GetContent());
  if (http_rsp.GetContent() != kGreetings) return false;
  return true;
}
}  // namespace

namespace {
//// HTTP unary invoke.

bool HttpUnaryInvoke(const HttpServiceProxyPtr& proxy) {
  auto ctx = ::trpc::MakeClientContext(proxy);
  ::trpc::http::HttpRequest http_req;
  http_req.SetMethod("POST");
  http_req.SetUrl("/foo");

  std::string req_content = R"({"msg": "hello world!"})";
  http_req.SetHeader("Content-Type", "application/json");
  http_req.SetHeader("Content-Length", std::to_string(req_content.size()));
  http_req.SetContent(req_content);

  ::trpc::http::HttpResponse http_rsp;
  auto status = proxy->HttpUnaryInvoke<::trpc::http::HttpRequest, ::trpc::http::HttpResponse>(ctx, http_req, &http_rsp);
  if (!status.OK()) {
    TRPC_FMT_ERROR("status: {}", status.ToString());
    return false;
  }
  TRPC_FMT_INFO("response content: {}", http_rsp.GetContent());
  if (http_rsp.GetContent() != req_content) return false;
  return true;
}
}  // namespace

int Run() {
  bool final_ok{true};

  struct http_calling_args_t {
    std::string calling_name;
    std::function<bool()> calling_executor;
    bool ok;
  };

  ::trpc::ServiceProxyOption option;
  option.name = FLAGS_service_name;
  option.codec_name = "http";
  option.network = "tcp";
  option.conn_type = "long";
  option.timeout = 1000;
  option.selector_name = "direct";
  option.target = FLAGS_addr;

  auto http_client = ::trpc::GetTrpcClient()->GetProxy<::trpc::http::HttpServiceProxy>(FLAGS_service_name, option);
  std::vector<http_calling_args_t> callings{

      // POST.
      {"POST json", [&http_client]() { return PostJson(http_client); }, false},

      // GET.
      {"GET string", [&http_client]() { return GetString(http_client); }, false},
  };

  auto latch_count = static_cast<std::ptrdiff_t>(callings.size());
  ::trpc::FiberLatch callings_latch{latch_count};

  for (auto& c : callings) {
    ::trpc::StartFiberDetached([&callings_latch, &c]() {
      c.ok = c.calling_executor();
      callings_latch.CountDown();
    });
  }

  callings_latch.Wait();

  for (const auto& c : callings) {
    final_ok &= c.ok;
    std::cout << "name: " << c.calling_name << ", ok: " << c.ok << std::endl;
  }

  std::cout << "final result of http calling: " << final_ok << std::endl;
  return final_ok ? 0 : -1;
}

}  // namespace http::demo

void ParseClientConfig(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::CommandLineFlagInfo info;
  if (GetCommandLineFlagInfo("client_config", &info) && info.is_default) {
    std::cerr << "start client with client_config, for example: " << argv[0]
              << " --client_config=/client/client_config/filepath" << std::endl;
    exit(-1);
  }
  std::cout << "FLAGS_service_name: " << FLAGS_service_name << std::endl;
  std::cout << "FLAGS_client_config: " << FLAGS_client_config << std::endl;
  std::cout << "FLAGS_addr: " << FLAGS_addr << std::endl;
}

int main(int argc, char* argv[]) {
  ParseClientConfig(argc, argv);

  if (::trpc::TrpcConfig::GetInstance()->Init(FLAGS_client_config) != 0) {
    std::cerr << "load client_config failed." << std::endl;
    exit(-1);
  }

  // If the business code is running in trpc pure client mode,
  // the business code needs to be running in the `RunInTrpcRuntime` function
  return ::trpc::RunInTrpcRuntime([]() { return http::demo::Run(); });
}
