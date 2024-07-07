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

#include <utility>

#include "trpc/common/trpc_app.h"

#include "examples/issueimpl/server/service.h"

namespace http::demo {

class HttpdServer : public ::trpc::TrpcApp {
 public:
  int Initialize() override {
    RegisterService("trpc.test.issueimpl.Introducer", std::make_shared<trpc::test::issueimpl::IntroducerServiceImpl>());

    return 0;
  }

  void Destroy() override {}
};

}  // namespace http::demo

int main(int argc, char* argv[]) {
  http::demo::HttpdServer httpd_server;

  httpd_server.Main(argc, argv);
  httpd_server.Wait();

  return 0;
}
