#include <stdio.h>
#include <iostream>

#include "../src/daemon_server.hpp"
#include "daemon.pb.h"

test_daemon::DaemonResponse handle(const test_daemon::DaemonRequest &request) {
  test_daemon::DaemonResponse out_response;
  std::cout << request.DebugString() << std::endl;
  out_response.set_str("Success!");
  return out_response;
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  daemon_manager::Server<test_daemon::DaemonRequest,
                         test_daemon::DaemonResponse>
      server("ipc:///tmp/some.ipc");
  server.register_callback(test_daemon::DaemonRequest::kNum, &handle);
  server.handle_request();
  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}
