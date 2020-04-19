#include "common.hpp"
#include "daemon_server.hpp"

namespace daemon_manager {

template <class TRequestMsg, class TResponseMsg, class TSocket>
void BaseServer<TRequestMsg, TResponseMsg, TSocket>::init(
    const std::string &request_endpoint,
    const std::string &subscribe_endpoint) {
  if (!request_endpoint.empty()) {
    m_request_socket.bind(request_endpoint);
  }
  if (!subscribe_endpoint.empty()) {
    m_publish_socket.bind(subscribe_endpoint);
  }
}

template <class TRequestMsg, class TResponseMsg, class TSocket>
void BaseServer<TRequestMsg, TResponseMsg, TSocket>::register_callback(
    const int cmd,
    std::function<TResponseMsg(const TRequestMsg &)> request_handler) {
  m_request_handlers[cmd] = request_handler;
}

template <class TRequestMsg, class TResponseMsg, class TSocket>
error_t BaseServer<TRequestMsg, TResponseMsg, TSocket>::publish(
    const TResponseMsg &msg) {
  std::string data;
  RETURN_IF_FAIL(m_response_serializer.encode(msg, data), EINVAL);
  RETURN_IF_FAIL(m_publish_socket.send(data), ECOMM);
  return 0;
}

template <class TRequestMsg, class TResponseMsg, class TSocket>

int BaseServer<TRequestMsg, TResponseMsg, TSocket>::get_fd(void) {
  int fd;
  m_request_socket.get(zmqpp::socket_option::file_descriptor, fd);
  return fd;
}

template <class TRequestMsg, class TResponseMsg, class TSocket>
std::function<TResponseMsg(const TRequestMsg &)>
BaseServer<TRequestMsg, TResponseMsg, TSocket>::find_handler(
    TRequestMsg &request) {
  std::function<TResponseMsg(const TRequestMsg &)> handler;
  auto handler_search =
      m_request_handlers.find(m_request_serializer.get_cmd_tag(request));
  if (handler_search != m_request_handlers.end()) {
    handler = handler_search->second;
  }
  return handler;
}

template <class TRequestMsg, class TResponseMsg, class TSocket>
error_t BaseServer<TRequestMsg, TResponseMsg, TSocket>::handle_request(void) {
  std::string data;
  TRequestMsg request;

  RETURN_IF_FAIL(m_request_socket.receive(data), ETIMEDOUT);
  RETURN_IF_FAIL(m_request_serializer.decode(data, request), EBADMSG);
  std::function<TResponseMsg(const TRequestMsg &)> handler =
      find_handler(request);
  RETURN_IF_FAIL(handler, EBADRQC);
  TResponseMsg response = handler(request);

  RETURN_IF_FAIL(m_response_serializer.encode(response, data), EINVAL);
  RETURN_IF_FAIL(m_request_socket.send(data), ECOMM);
  return 0;
}

} // namespace daemon_manager
