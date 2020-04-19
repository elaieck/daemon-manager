import pydaemon
import daemon_pb2


def main():
    client = pydaemon.daemon_subscriber(daemon_pb2.DaemonResponse,
                                        "ipc:///tmp/test.ipc")
    msg = client.receive()
    print msg.choice


if __name__ == "__main__":
    main()
