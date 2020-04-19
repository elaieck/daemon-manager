import pydaemon
import daemon_pb2


def main():
    client = pydaemon.daemon_request_client(daemon_pb2.DaemonRequest,
                                            daemon_pb2.DaemonResponse,
                                            "ipc:///tmp/test.ipc")
    request = daemon_pb2.DaemonRequest()
    request.str = "Boo"
    response = client.request(request)
    print response.str


if __name__ == "__main__":
    main()
