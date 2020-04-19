import pydaemon
import daemon_pb2


def handle(request):
    print request.str
    response = daemon_pb2.DaemonResponse()
    response.str = "Yah!"
    return response


def main():
    server = pydaemon.daemon_server(daemon_pb2.DaemonRequest,
                                    daemon_pb2.DaemonResponse,
                                    "ipc:///tmp/test.ipc")
    server.register_callback(daemon_pb2.DaemonRequest.str.DESCRIPTOR.number,
                             handle)
    server.handle_request()


if __name__ == "__main__":
    main()
