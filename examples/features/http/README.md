# HTTP Unary calling (Request-Response).

In HTTP request-response calling, both the client and the server can send requests and responses.
The client uses HTTP request methods (such as GET, POST, etc.) to send requests, and specifies the requested URL,
HTTP protocol version, request headers, request body and other related information. The server receives the request,
processes it, and generates an HTTP response, which includes HTTP status code, response headers, response body, and
other related information. After the request is processed, the HTTP connection is automatically closed, and the
interaction between the client and the server ends.

## Usage

We can use the following command to view the directory tree.
```shell
$ examples/features/http/
examples/features/http/
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
sh examples/features/http/run.sh
```

* Compilation

We can run the following command to compile the client and server programs.

```shell
bazel build  //examples/features/http/server:http_server
bazel build  //examples/features/http/client:client
```

Alternatively, you can use cmake.
```shell
# build trpc-cpp libs first, if already build, just skip this build process.
$ mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j8 && cd -
# build examples/http
$ mkdir -p examples/features/http/build && cd examples/features/http/build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j8 && cd -
```

* Run the server program

We can run the following command to start the server program.

*CMake build targets can be found at `build` of this directory, you can replace below server&client binary path when you use cmake to compile.*

```shell
bazel-bin/examples/features/http/server/http_server --config=examples/features/http/server/trpc_cpp_fiber.yaml
```

* Run the client program

We can run the following command to start the client program.

```shell
bazel-bin/examples/features/http/client/client --client_config=examples/features/http/client/trpc_cpp_fiber.yaml
```

The content of the output from the client program is as follows:
``` text
FLAGS_service_name: http_client
FLAGS_client_config: examples/features/http/client/trpc_cpp_fiber.yaml
FLAGS_addr: 127.0.0.1:24856
logging plugin configurations is not setted, log will be printed to the console.
response content: {"name":"issueshooter","age":18,"hobby":["opensource project","movies","books"]}
response content: {"msg":"your name is issueshooter who likes opensource project mostly."}
name: POST json, ok: 1
name: GET string, ok: 1
final result of http calling: 1
```

* Use curl command to test

first, use the following command to run thr http-server, then open another terminal window
```shell
bazel-bin/examples/features/http/server/http_server --config=examples/features/http/server/trpc_cpp_fiber.yaml
```
if you run the command in the other terminal window
```shell
curl http://0.0.0.0:24856/issueshoot-test -X POST -H "Content-Type: application/json" -d '{"name": "issueshooter", "age": 18, "hobby": ["opensource project", "movies", "books"]}'
```
the expected output is
```text
{"name": "issueshooter", "age": 18, "hobby": ["opensource project", "movies", "books"]}
```
then run the following command
```shell
curl http://0.0.0.0:24856/hello -X GET
```
the expected output is
```text
{"msg":"your name is issueshooter who likes opensource project mostly."}
```