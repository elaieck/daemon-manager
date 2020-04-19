#include <iostream>

#include "../src/daemon_client.hpp"
#include "daemon.pb.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  test_daemon::DaemonRequest daemon_request;
  test_daemon::DaemonResponse daemon_response;
  daemon_manager::RequestClient<test_daemon::DaemonRequest,
                                test_daemon::DaemonResponse>
      client("ipc:///tmp/some.ipc");
  daemon_request.set_num(1337);
  client.request(daemon_request, daemon_response);
  std::cout << daemon_response.DebugString() << std::endl;
  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}
