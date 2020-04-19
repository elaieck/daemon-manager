#ifndef __MOCK_SOCKET_HPP__
#define __MOCK_SOCKET_HPP__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <zmqpp/zmqpp.hpp>

class MockSocket {
public:
  MockSocket(zmqpp::context_t const &context, zmqpp::socket_type const type) {}
  MOCK_METHOD1(connect, void(std::string const &));
  MOCK_METHOD1(bind, void(std::string const &));
  MOCK_METHOD1(send, bool(std::string const &));
  MOCK_METHOD1(receive, bool(std::string &));
  MOCK_METHOD1(subscribe, void(std::string const &));
  MOCK_METHOD2(get, void(zmqpp::socket_option const, int &));
};

#endif // __MOCK_SOCKET_HPP__
