#include "server/service.h"

#include "trpc/log/trpc_log.h"


#include "trpc/util/http/http_handler.h"

namespace trpc {
namespace test {
namespace issueimpl {

::trpc::Status IntroducerServiceImpl::SelfIntroduction(::trpc::ServerContextPtr context, const ::trpc::test::issueimpl::IntroduceRequest* request, ::trpc::test::issueimpl::IntroduceReply* reply) {
  // Your can access more information from rpc context, eg: remote ip and port
  TRPC_FMT_INFO("remote address: {}:{}", context->GetIp(), context->GetPort());
  TRPC_FMT_INFO("request message: {}", request->msg());

  // Implement business logic here
  TRPC_FMT_INFO("got req");
  rapidjson::Document res_json;
  res_json.Parse(request->msg().c_str());

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
  reply->set_msg(buffer.GetString());
  return ::trpc::kSuccStatus;
}

}  // namespace issueimpl
}  // namespace test
}  // namespace trpc
