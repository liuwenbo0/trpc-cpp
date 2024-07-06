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

#include <memory>
#include <string>
#include <utility>

#include "trpc/common/trpc_app.h"
#include "trpc/server/http_service.h"
#include "trpc/util/http/function_handlers.h"
#include "trpc/util/http/http_handler.h"
#include "trpc/util/http/routes.h"
#include "trpc/util/log/logging.h"

namespace http::demo {

// An easy way to implement an echo service.
class HelloHandler : public ::trpc::http::HttpHandler {
 public:
  ::trpc::Status Handle(const ::trpc::ServerContextPtr& ctx, const ::trpc::http::RequestPtr& req,
                        ::trpc::http::Response* rsp) override {
    // Prints the complete request information, including request line, request headers, and request body.
    TRPC_LOG_INFO("request: " << req->SerializeToString());
    // HTTP HEAD.
    if (req->GetMethodType() == ::trpc::http::MethodType::HEAD) {
      rsp->SetHeader("Content-Length", "100");
    } else if (req->GetMethodType() == ::trpc::http::MethodType::POST) {
      rsp->SetContent(req->GetContent());
    } else if (req->GetMethodType() == ::trpc::http::MethodType::GET) {
      std::string response_json = R"({"msg": "hello world!", "age": 18, "height": 180})";
      rsp->SetContent(std::move(response_json));
    } else {
      rsp->SetStatus(::trpc::http::ResponseStatus::kNotImplemented);
    }
    return ::trpc::kSuccStatus;
  }
};

// Another easy way to handle HTTP requests.
class FooHandler : public ::trpc::http::HttpHandler {
 public:
  ::trpc::Status Get(const ::trpc::ServerContextPtr& ctx, const ::trpc::http::RequestPtr& req,
                     ::trpc::http::Response* rsp) override {
    TRPC_LOG_INFO("request: " << req->SerializeToString());
    rsp->SetContent(greetings_);
    return ::trpc::kSuccStatus;
  }
  ::trpc::Status Head(const ::trpc::ServerContextPtr& ctx, const ::trpc::http::RequestPtr& req,
                      ::trpc::http::Response* rsp) override {
    TRPC_LOG_INFO("request: " << req->SerializeToString());
    rsp->SetHeader("Content-Length", std::to_string(greetings_.size()));
    return ::trpc::kSuccStatus;
  }
  ::trpc::Status Post(const ::trpc::ServerContextPtr& ctx, const ::trpc::http::RequestPtr& req,
                      ::trpc::http::Response* rsp) override {
    TRPC_LOG_INFO("request: " << req->SerializeToString());
    rsp->SetContent(req->GetContent());
    return ::trpc::kSuccStatus;
  }

 private:
  std::string greetings_{"hello world!"};
};

// Another easy way to handle HTTP requests.
class IssueHandler : public ::trpc::http::HttpHandler {
 public:
  ::trpc::Status Get(const ::trpc::ServerContextPtr& ctx, const ::trpc::http::RequestPtr& req,
                     ::trpc::http::Response* rsp) override {
    TRPC_LOG_INFO("request: " << req->SerializeToString());
    rapidjson::Document res_json;
    res_json.Parse(messages_.c_str());

    // read the `res_json`, construct reponse message `response_stmt`
    std::string response_stmt{"your name is "};
    if (res_json.HasParseError()) return ::trpc::kUnknownErrorStatus;

    if (res_json.HasMember("name") && res_json["name"].IsString()) response_stmt += res_json["name"].GetString();
    response_stmt += " who likes ";
    if (!res_json.HasMember("hobby") || !res_json["hobby"].IsArray()) return ::trpc::kUnknownErrorStatus;
    const rapidjson::Value& hobbyArray = res_json["hobby"];
    if (hobbyArray.Empty() || !hobbyArray[0].IsString()) return ::trpc::kUnknownErrorStatus;
    response_stmt += hobbyArray[0].GetString();
    response_stmt += " mostly.";

    // construct response json `rsp_json`
    rapidjson::Document rsp_json;
    rsp_json.SetObject();
    rapidjson::Document::AllocatorType& allocator = rsp_json.GetAllocator();

    rsp_json.AddMember("msg", rapidjson::Value().SetString(response_stmt.c_str(), allocator), allocator);

    // convert response json `rsp_json` into string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    rsp_json.Accept(writer);
    rsp->SetContent(buffer.GetString());
    return ::trpc::kSuccStatus;
  }

  // namespace http::demo
  ::trpc::Status Head(const ::trpc::ServerContextPtr& ctx, const ::trpc::http::RequestPtr& req,
                      ::trpc::http::Response* rsp) override {
    TRPC_LOG_INFO("request: " << req->SerializeToString());
    rsp->SetHeader("Content-Length", std::to_string(messages_.size()));
    return ::trpc::kSuccStatus;
  }
  ::trpc::Status Post(const ::trpc::ServerContextPtr& ctx, const ::trpc::http::RequestPtr& req,
                      ::trpc::http::Response* rsp) override {
    TRPC_LOG_INFO("request: " << req->SerializeToString());
    messages_ = req->GetContent();
    rsp->SetContent(req->GetContent());
    return ::trpc::kSuccStatus;
  }

 private:
  std::string messages_{""};
};

void SetHttpRoutes(::trpc::http::HttpRoutes& r) {
  auto hello_handler = std::make_shared<HelloHandler>();
  // Matches exactly.
  r.Add(::trpc::http::MethodType::GET, ::trpc::http::Path("/hello"), hello_handler);
  r.Add(::trpc::http::MethodType::HEAD, ::trpc::http::Path("/hello"), hello_handler);
  r.Add(::trpc::http::MethodType::POST, ::trpc::http::Path("/hello"), hello_handler);
  // Matches prefix.
  r.Add(::trpc::http::MethodType::POST, ::trpc::http::Path("/hi").Remainder("path"), hello_handler);

  auto foo_handler = std::make_shared<FooHandler>();
  r.Add(::trpc::http::MethodType::GET, ::trpc::http::Path("/foo"), foo_handler);
  r.Add(::trpc::http::MethodType::HEAD, ::trpc::http::Path("/foo"), foo_handler);
  r.Add(::trpc::http::MethodType::POST, ::trpc::http::Path("/foo"), foo_handler);

  auto issue_handler = std::make_shared<IssueHandler>();
  r.Add(::trpc::http::MethodType::GET, ::trpc::http::Path("/issueshoot-test"), issue_handler);
  r.Add(::trpc::http::MethodType::HEAD, ::trpc::http::Path("/issueshoot-test"), issue_handler);
  r.Add(::trpc::http::MethodType::POST, ::trpc::http::Path("/issueshoot-test"), issue_handler);
}

class HttpdServer : public ::trpc::TrpcApp {
 public:
  int Initialize() override {
    auto http_service = std::make_shared<::trpc::HttpService>();
    http_service->SetRoutes(SetHttpRoutes);

    RegisterService("default_http_service", http_service);

    return 0;
  }

  void Destroy() override {}
};

}  // namespace http::demo

int main(int argc, char** argv) {
  http::demo::HttpdServer httpd_server;

  httpd_server.Main(argc, argv);
  httpd_server.Wait();

  return 0;
}
