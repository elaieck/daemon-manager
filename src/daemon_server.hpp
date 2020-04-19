#ifndef __DAEMON_SERVER_H__
#define __DAEMON_SERVER_H__

#include <errno.h>
#include <zmqpp/zmqpp.hpp>
#include "serializer.hpp"

namespace daemon_manager {

template <class TRequestMsg, class TResponseMsg, class TSocket>
class BaseServer {
public:
  BaseServer(Serializer<TRequestMsg> &request_serializer,
             Serializer<TResponseMsg> &response_serializer,
             TSocket &request_socket, TSocket &publish_socket)
      : m_request_serializer(request_serializer),
        m_response_serializer(response_serializer),
        m_request_socket(request_socket), m_publish_socket(publish_socket){};

  // Initialize chosen sockets.
  void init(const std::string &request_endpoint = "",
            const std::string &subscribe_endpoint = "");

  // Add a new command to the server's command list.
  // Every time the client would send a request to the server, the server would
  // call the the callback - "request_handler" that was registered with the same
  // tag - "cmd".
  void register_callback(
      const int cmd,
      std::function<TResponseMsg(const TRequestMsg &)> request_handler);

  // Send a message to all listening client (requires a subscribe endpoint).
  // Return Values: EINVAL - cannot encode msg
  //                ECOMM - failed to send message
  error_t publish(const TResponseMsg &msg);

  // Get the file descriptor of the request socket.
  int get_fd(void);

  // Receive a request from a client, call the matching request handler and
  // send back the returning response from the callback (requires a
  // request endpoint).
  // Return Values: ETIMEDOUT - failed to receive request message
  //                EBADMSG - cannot decode request message
  //                EBADRQC - Got an unregistered request tag
  //                EINVAL - cannot encode response message
  //                ECOMM - failed to send response message
  error_t handle_request(void);

protected:
  Serializer<TRequestMsg> &m_request_serializer;
  Serializer<TResponseMsg> &m_response_serializer;
  TSocket &m_request_socket;
  TSocket &m_publish_socket;
  std::map<int, std::function<TResponseMsg(const TRequestMsg &)>>
      m_request_handlers;

  // find the callback that was registered to "request"'s tag.
  std::function<TResponseMsg(const TRequestMsg &)>
  find_handler(TRequestMsg &request);
};

// An implementation of BaseServer with zmqpp socket as the socket used
// and Google Protobuf as the serializer.
template <class TRequestMsg, class TResponseMsg>
class Server : public BaseServer<TRequestMsg, TResponseMsg, zmqpp::socket> {
public:
  Server(const std::string &request_endpoint = "",
         const std::string &subscribe_endpoint = "")
      : m_context(), m_request_socket(m_context, zmqpp::socket_type::rep),
        m_publish_socket(m_context, zmqpp::socket_type::pub),
        BaseServer<TRequestMsg, TResponseMsg, zmqpp::socket>(
            m_request_serializer, m_response_serializer, m_request_socket,
            m_publish_socket) {
    this->init(request_endpoint, subscribe_endpoint);
  }

protected:
  PbSerializer<TRequestMsg> m_request_serializer;
  PbSerializer<TResponseMsg> m_response_serializer;
  zmqpp::context m_context;
  zmqpp::socket m_request_socket;
  zmqpp::socket m_publish_socket;
};

} // namespace daemon_manager

#include "daemon_server.ipp"
#endif /* __DAEMON_SERVER_H__ */
