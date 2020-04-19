#ifndef __SERIALIZER_HPP__
#define __SERIALIZER_HPP__

namespace daemon_manager {

template <class TMessage> class Serializer {
public:
  Serializer(void) {}

  // Decode "data" to a message object form, and store it in "msg"
  // Return whether it succeeded or not
  virtual bool decode(std::string const &data, TMessage &msg) = 0;

  // Encode "msg" to a serialized string form, and store it in "data"
  // Return whether it succeeded or not
  virtual bool encode(TMessage const &msg, std::string &data) = 0;

  // Get the the numeric value of "request"'s command tag
  virtual int get_cmd_tag(TMessage const &request) = 0;
};

template <class TMessage> class PbSerializer : public Serializer<TMessage> {
public:
  using Serializer<TMessage>::Serializer;
  bool decode(std::string const &data, TMessage &msg) {
    return msg.ParseFromString(data);
  }
  bool encode(TMessage const &msg, std::string &data) {
    return msg.SerializeToString(&data);
  }
  int get_cmd_tag(TMessage const &request) { return request.cmd_case(); }
};

} // namespace daemon_manager

#endif /* __SERIALIZER_HPP__ */
