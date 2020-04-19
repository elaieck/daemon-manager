#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/daemon_client.hpp"
#include "mock_socket.hpp"
#include "mock_serializer.hpp"
#include "../build/tests/daemon.pb.h"

using ::testing::_;
using ::testing::SetArgReferee;
using ::testing::DoAll;
using ::testing::Ref;
using ::testing::Return;

using namespace daemon_manager;
using test_daemon::DaemonRequest;
using test_daemon::DaemonResponse;

class RequestClientTest : public ::testing::Test {
public:
  RequestClientTest()
      : m_mock_request_serializer(), m_mock_response_serializer(), m_context(),
        m_mock_socket(m_context, zmqpp::socket_type::req),
        m_daemon_client(m_mock_request_serializer, m_mock_response_serializer,
                        m_mock_socket) {
    EXPECT_CALL(m_mock_socket, connect(_)).Times(1);
    m_daemon_client.init("example.ipc");
  }

protected:
  MockSerializer<DaemonRequest> m_mock_request_serializer;
  MockSerializer<DaemonResponse> m_mock_response_serializer;
  zmqpp::context m_context;
  MockSocket m_mock_socket;
  BaseRequestClient<DaemonRequest, DaemonResponse, MockSocket> m_daemon_client;
  DaemonRequest m_request;
  DaemonResponse m_response;
};

TEST_F(RequestClientTest, test_request) {
  EXPECT_CALL(m_mock_request_serializer, encode(_, _)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_socket, send(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_socket, receive(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_response_serializer, decode(_, _)).WillOnce(Return(true));
  ASSERT_EQ(m_daemon_client.request(m_request, m_response), 0);
};

TEST_F(RequestClientTest, test_request_serialization_failed) {
  EXPECT_CALL(m_mock_request_serializer, encode(_, _)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_client.request(m_request, m_response), EINVAL);
};

TEST_F(RequestClientTest, test_request_failed_to_send) {
  EXPECT_CALL(m_mock_request_serializer, encode(_, _)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_socket, send(_)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_client.request(m_request, m_response), ECOMM);
};

TEST_F(RequestClientTest, test_request_recv_got_timeout) {
  EXPECT_CALL(m_mock_request_serializer, encode(_, _)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_socket, send(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_socket, receive(_)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_client.request(m_request, m_response), ETIMEDOUT);
};

TEST_F(RequestClientTest, test_request_failed_to_parse_message) {
  EXPECT_CALL(m_mock_request_serializer, encode(_, _)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_socket, send(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_socket, receive(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_response_serializer, decode(_, _)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_client.request(m_request, m_response), EBADMSG);
};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
