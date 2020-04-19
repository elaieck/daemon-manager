import unittest
import pydaemon
import daemon_pb2
import multiprocessing

IPC_FILE = "ipc:///tmp/test.ipc"
REQUEST_MSG = "request_example"
RESPONSE_MSG = "response_example"


class TestableServer(multiprocessing.Process):

    def handle(self, request):
        assert request.str == REQUEST_MSG
        return daemon_pb2.DaemonResponse(str=RESPONSE_MSG)

    def run(self):
        server = pydaemon.daemon_server(daemon_pb2.DaemonRequest,
                                        daemon_pb2.DaemonResponse,
                                        IPC_FILE)
        server.register_callback(daemon_pb2.DaemonRequest.str.DESCRIPTOR.number,
                                 self.handle)
        server.handle_request()


class TestableClient(multiprocessing.Process):

    def run(self):
        client = pydaemon.daemon_request_client(daemon_pb2.DaemonRequest,
                                                daemon_pb2.DaemonResponse,
                                                IPC_FILE)
        request = daemon_pb2.DaemonRequest(str=REQUEST_MSG)
        response = client.request(request)
        assert response.str == RESPONSE_MSG


class DaemonRequestResponseTest(unittest.TestCase):

    def setUp(self):
        self.server = TestableServer()
        self.client = TestableClient()

    def test_daemons(self):
        self.server.start()
        self.client.start()
        self.server.join()
        self.client.join()


if __name__ == "__main__":
    unittest.main()
