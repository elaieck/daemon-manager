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

class SubscriberTest : public ::testing::Test {
public:
  SubscriberTest()
      : m_mock_response_serializer(), m_context(),
        m_mock_socket(m_context, zmqpp::socket_type::req),
        m_daemon_subscriber(m_mock_response_serializer, m_mock_socket) {
    EXPECT_CALL(m_mock_socket, connect(_)).Times(1);
    EXPECT_CALL(m_mock_socket, subscribe(_)).Times(1);
    m_daemon_subscriber.init("example.ipc", "some_topic");
  }

protected:
  MockSerializer<DaemonResponse> m_mock_response_serializer;
  zmqpp::context m_context;
  MockSocket m_mock_socket;
  BaseSubscriber<DaemonResponse, MockSocket> m_daemon_subscriber;
  DaemonResponse m_response;
};

TEST_F(SubscriberTest, test_receive) {
  EXPECT_CALL(m_mock_socket, receive(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_response_serializer, decode(_, _)).WillOnce(Return(true));
  ASSERT_EQ(m_daemon_subscriber.receive(m_response), 0);
};

TEST_F(SubscriberTest, test_receive_socket_failed_to_recv) {
  EXPECT_CALL(m_mock_socket, receive(_)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_subscriber.receive(m_response), ETIMEDOUT);
};

TEST_F(SubscriberTest, test_receive_fail_to_parse) {
  EXPECT_CALL(m_mock_socket, receive(_)).WillOnce(Return(true));
  EXPECT_CALL(m_mock_response_serializer, decode(_, _)).WillOnce(Return(false));
  ASSERT_EQ(m_daemon_subscriber.receive(m_response), EBADMSG);
};

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
