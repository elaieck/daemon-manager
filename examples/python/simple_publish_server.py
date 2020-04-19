import time
import pydaemon
import daemon_pb2


def main():
    server = pydaemon.daemon_server(daemon_pb2.DaemonRequest,
                                    daemon_pb2.DaemonResponse,
                                    subscribe_endpoint="ipc:///tmp/test.ipc")
    msg = daemon_pb2.DaemonResponse()
    msg.choice = True
    time.sleep(1)
    server.publish(msg)


if __name__ == "__main__":
    main()
