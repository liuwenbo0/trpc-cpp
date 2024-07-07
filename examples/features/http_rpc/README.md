# RPC over HTTP Unary calling (Request-Response).

In the RPC over HTTP Unary Calling scenario, the server provides RPC services using the HTTP protocol, and the
client sends ProtoBuffers messages using the HTTP protocol, which is very convenient.

There are currently the following restrictions: only tRPC RPC services are supported.

## Usage

We can use the following command to view the directory tree.

```shell
$ tree examples/features/http_rpc/
examples/features/http_rpc/
├── client
│   ├── BUILD
│   ├── client.cc
│   └── trpc_cpp_fiber.yaml
├── CMakeLists.txt
├── README.md
├── run_cmake.sh
├── run.sh
└── server
    ├── BUILD
    ├── http_server.cc
    └── trpc_cpp_fiber.yaml
```

We can use the following script to quickly compile and run a program.

```shell
sh examples/features/http_rpc/run.sh
```

* Compilation

We can run the following command to compile the client and server programs.

```shell
bazel build  //examples/features/http_rpc/server:http_rpc_server
bazel build  //examples/features/http_rpc/client:client
```

Alternatively, you can use cmake.
```shell
# build trpc-cpp libs first, if already build, just skip this build process.
$ mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j8 && cd -
# build examples/http_rpc
$ mkdir -p examples/features/http_rpc/build && cd examples/features/http_rpc/build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j8 && cd -
```

* Run the server program

We can run the following command to start the server program.

*CMake build targets can be found at `build` of this directory, you can replace below server&client binary path when you use cmake to compile.*

```shell
bazel-bin/examples/features/http_rpc/server/http_rpc_server --config=examples/features/http_rpc/server/trpc_cpp_fiber.yaml
```

* Run the client program

We can run the following command to start the client program.

```shell
bazel-bin/examples/features/http_rpc/client/client --client_config=examples/features/http_rpc/client/trpc_cpp_fiber.yaml
```

The content of the output from the client program is as follows:

``` text
FLAGS_service_name: http_client
FLAGS_client_config: examples/features/http_rpc/client/trpc_cpp_fiber.yaml
FLAGS_addr: 127.0.0.1:54320
logging plugin configurations is not setted, log will be printed to the console.
request content: {"name":"issueshooter","age":18,"hobby":["opensource project","movies","books"]}
request content: {"name":"issueshooter","age":18,"hobby":["opensource project","movies","books"]}
request content: {"name":"issueshooter","age":18,"hobby":["opensource project","movies","books"]}
response content: {"msg":"your name is issueshooter who likes opensource project mostly."}
response content: {"msg":"your name is issueshooter who likes opensource project mostly."}
response content: {"msg":"your name is issueshooter who likes opensource project mostly."}
name: Unary invoking-1, ok: 1
name: Unary invoking-2, ok: 1
name: Unary invoking-3, ok: 1
final result of http calling: 1
```

* Use protocurl command to test

install the protocurl cli tool first, execute the following command
```shell
wget https://github.com/qaware/protocurl/releases/download/v1.9.0/protocurl_1.9.0_linux_386.zip
mkdir -p /tmp/protocurl
unzip protocurl_1.9.0_linux_386.zip -d /tmp/protocurl
rm -f protocurl_1.9.0_linux_386.zip
mv /tmp/protocurl/bin/protocurl /usr/local/bin/
mv /tmp/protocurl/protocurl-internal /usr/local/
chmod +x /usr/local/bin/protocurl
rm -rf /tmp/protocurl
protocurl -h
```

use the following command to run thr http-server, then open another terminal window
```shell
bazel-bin/examples/features/http_rpc/server/http_rpc_server --config=examples/features/http_rpc/server/trpc_cpp_fiber.yaml
```
if you run the command in the other terminal window
```shell
protocurl -I examples/issueimpl/proto -f issueimpl.proto -i trpc.test.issueimpl.IntroduceRequest -o trpc.test.issueimpl.IntroduceReply --in 'json' -d '{"msg": "{\"name\":\"issueshooter\",\"age\":18,\"hobby\":[\"opensource project\",\"movies\",\"books\"]}"}' -u http://0.0.0.0:54320/trpc.test.issueimpl.Introducer/SelfIntroduction
```
the expected output is
```text
=========================== POST Request  JSON    =========================== >>>
{"msg":"{\"name\":\"issueshooter\",\"age\":18,\"hobby\":[\"opensource project\",\"movies\",\"books\"]}"}
=========================== POST Response JSON    =========================== <<<
{"msg":"{\"msg\":\"your name is issueshooter who likes opensource project mostly.\"}"}
```