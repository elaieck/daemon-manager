#ifndef __MOCK_SERIALIZER_HPP__
#define __MOCK_SERIALIZER_HPP__

#include <gmock/gmock.h>
#include <daemon_manager/serializer.hpp>

template <class message>
class MockSerializer : public daemon_manager::Serializer<message> {
public:
  MockSerializer(void) {}

  MOCK_METHOD2_T(decode, bool(std::string const &, message &));
  MOCK_METHOD2_T(encode, bool(message const &, std::string &));
  MOCK_METHOD1_T(get_cmd_tag, int(message const &));
};

#endif // __MOCK_SERIALIZER_HPP__
