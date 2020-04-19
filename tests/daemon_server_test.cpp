#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "mock_serializer.hpp"
#include "mock_socket.hpp"
#include <daemon.pb.h>
#include <daemon_manager/daemon_server.hpp>

using ::testing::_;
using ::testing::SetArgReferee;
using ::testing::DoAll;
using ::testing::Ref;
using ::testing::Return;

using namespace daemon_manager;
using test_daemon::DaemonRequest;
using test_daemon::DaemonResponse;

class ServerTest : public ::testing::Test {
public:
  ServerTest()
      : m_mock_request_serializer(), m_mock_response_serializer(), m_context(),
        m_mock_request_socket(m_context, zmqpp::socket_type::rep),
        m_mock_publish_socket(m_context, zmqpp::socket_type::pub),
        m_daemon_server(m_mock_request_serializer, m_mock_response_serializer,
                        m_mock_request_socket, m_mock_publish_socket) {
    EXPECT_CALL(m_mock_request_socket, bind(_)).Times(1);
    EXPECT_CALL(m_mock_publish_socket, bind(_)).Times(1);
    m_daemon_server.init("example1.ipc", "example2.ipc");
  }

  DaemonResponse handle(const DaemonRequest &request) {
    m_command_called = request.cmd_case();
    DaemonResponse out_response;
    out_response.set_str("success");
    return out_response;
  }

protected:
  MockSerializer<DaemonRequest> m_mock_request_serializer;
  MockSerializer<DaemonResponse> m_mock_response_serializer;
  zmqpp::context m_context;
  MockSocket m_mock_request_socket;
  MockSocket m_mock_publish_socket;
  BaseServer<DaemonRequest, DaemonResponse, MockSocket> m_daemon_server;
  DaemonRequest m_request;
  DaemonResponse m_response;
  int m_command_called = DaemonRequest::CMD_NOT_SET;
};

TEST_F(ServerTest, test_handle_request) {
  m_daemon_server.register_callback(
      DaemonRequest::kNum,
      std::bind(&ServerTest::handle, this, std::placeholders::_1));
  m_daemon_server.register_callback(
      DaemonRequest::kChoice,
      std::bind(&ServerTest::handle, this, std::placeholders::_1));

  m_request.set_choice(true);

  EXPECT_CALL(m_mock_request_socket, receive(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_request_serializer, decode(_, _))
      .WillOnce(DoAll(SetArgReferee<1>(m_request), Return(true)));
  EXPECT_CALL(m_mock_request_serializer, get_cmd_tag(_))
      .WillOnce(Return(m_request.cmd_case()));
  EXPECT_CALL(m_mock_response_serializer, encode(_, _)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_request_socket, send(_)).WillOnce(Return(true));
  ASSERT_EQ(m_daemon_server.handle_request(), 0);

  ASSERT_EQ(m_command_called, DaemonRequest::kChoice);
};

TEST_F(ServerTest, test_handle_request_recv_got_timeout) {
  EXPECT_CALL(m_mock_request_socket, receive(_)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_server.handle_request(), ETIMEDOUT);
};

TEST_F(ServerTest, test_handle_request_recv_bad_data) {
  EXPECT_CALL(m_mock_request_socket, receive(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_request_serializer, decode(_, _)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_server.handle_request(), EBADMSG);
};

TEST_F(ServerTest, test_handle_request_recv_unregistered_cmd) {
  std::string data;
  EXPECT_CALL(m_mock_request_socket, receive(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_request_serializer, decode(_, _))
      .WillOnce(DoAll(SetArgReferee<1>(m_request), Return(true)));
  EXPECT_CALL(m_mock_request_serializer, get_cmd_tag(_))
      .WillOnce(Return(m_request.cmd_case()));
  ASSERT_EQ(m_daemon_server.handle_request(), EBADRQC);
};

TEST_F(ServerTest, test_handle_request_serialization_fail) {
  m_daemon_server.register_callback(
      DaemonRequest::kChoice,
      std::bind(&ServerTest::handle, this, std::placeholders::_1));
  m_request.set_choice(true);

  EXPECT_CALL(m_mock_request_socket, receive(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_request_serializer, decode(_, _))
      .WillOnce(DoAll(SetArgReferee<1>(m_request), Return(true)));
  EXPECT_CALL(m_mock_request_serializer, get_cmd_tag(_))
      .WillOnce(Return(m_request.cmd_case()));
  EXPECT_CALL(m_mock_response_serializer, encode(_, _)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_server.handle_request(), EINVAL);
};

TEST_F(ServerTest, test_handle_request_connection_fail_on_send) {
  m_daemon_server.register_callback(
      DaemonRequest::kChoice,
      std::bind(&ServerTest::handle, this, std::placeholders::_1));
  m_request.set_choice(true);

  EXPECT_CALL(m_mock_request_socket, receive(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_request_serializer, decode(_, _))
      .WillOnce(DoAll(SetArgReferee<1>(m_request), Return(true)));
  EXPECT_CALL(m_mock_request_serializer, get_cmd_tag(_))
      .WillOnce(Return(m_request.cmd_case()));
  EXPECT_CALL(m_mock_response_serializer, encode(_, _)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_request_socket, send(_)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_server.handle_request(), ECOMM);
};

TEST_F(ServerTest, test_publish) {
  EXPECT_CALL(m_mock_response_serializer, encode(_, _)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_publish_socket, send(_)).WillOnce(Return(true));
  ASSERT_EQ(m_daemon_server.publish(m_response), 0);
};

TEST_F(ServerTest, test_publish_serialization_fail) {
  EXPECT_CALL(m_mock_response_serializer, encode(_, _)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_server.publish(m_response), EINVAL);
};

TEST_F(ServerTest, test_publish_connection_fail) {
  EXPECT_CALL(m_mock_response_serializer, encode(_, _)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_publish_socket, send(_)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_server.publish(m_response), ECOMM);
};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
