#include "common.hpp"
#include "daemon_client.hpp"

namespace daemon_manager {

template <class TRequestMsg, class TResponseMsg, class TSocket>
void BaseRequestClient<TRequestMsg, TResponseMsg, TSocket>::init(
    const std::string &endpoint) {
  m_socket.connect(endpoint);
}

template <class TRequestMsg, class TResponseMsg, class TSocket>
error_t BaseRequestClient<TRequestMsg, TResponseMsg, TSocket>::request(
    const TRequestMsg &msg, TResponseMsg &response) {
  std::string data;
  RETURN_IF_FAIL(m_request_serializer.encode(msg, data), EINVAL);
  RETURN_IF_FAIL(m_socket.send(data), ECOMM);
  RETURN_IF_FAIL(m_socket.receive(data), ETIMEDOUT);
  RETURN_IF_FAIL(m_response_serializer.decode(data, response), EBADMSG);
  return 0;
}

template <class TResponseMsg, class TSocket>
void BaseSubscriber<TResponseMsg, TSocket>::init(const std::string &endpoint,
                                                 const std::string &topic) {
  m_socket.connect(endpoint);
  m_socket.subscribe(topic);
}

template <class TResponseMsg, class TSocket>
int BaseSubscriber<TResponseMsg, TSocket>::get_fd(void) {
  int fd;
  m_socket.get(zmqpp::socket_option::file_descriptor, fd);
  return fd;
}

template <class TResponseMsg, class TSocket>
error_t BaseSubscriber<TResponseMsg, TSocket>::receive(TResponseMsg &msg) {
  std::string data;
  RETURN_IF_FAIL(m_socket.receive(data), ETIMEDOUT);
  RETURN_IF_FAIL(m_response_serializer.decode(data, msg), EBADMSG);
  return 0;
}

} // namespace daemon_manager
