#ifndef __DAEMON_CLIENT_H__
#define __DAEMON_CLIENT_H__

#include <errno.h>
#include <zmqpp/zmqpp.hpp>
#include "serializer.hpp"

namespace daemon_manager {

template <class TRequestMsg, class TResponseMsg, class TSocket>
class BaseRequestClient {
public:
  BaseRequestClient(Serializer<TRequestMsg> &request_serializer,
                    Serializer<TResponseMsg> &response_serializer,
                    TSocket &request_socket)
      : m_request_serializer(request_serializer),
        m_response_serializer(response_serializer), m_socket(request_socket){};

  // Initialize connection.
  void init(const std::string &endpoint);

  // Send a request to the server. "response" is an output argument that
  // will contain the server's response message.
  // Return Values: EINVAL - cannot encode request message
  //                ECOMM - failed to send request message
  //                ETIMEDOUT - failed to receive response
  //                EBADMSG - cannot decode response message
  error_t request(const TRequestMsg &msg, TResponseMsg &response);

protected:
  Serializer<TRequestMsg> &m_request_serializer;
  Serializer<TResponseMsg> &m_response_serializer;
  TSocket &m_socket;
};

template <class TResponseMsg, class TSocket> class BaseSubscriber {
public:
  BaseSubscriber(Serializer<TResponseMsg> &response_serializer,
                 TSocket &subscribe_socket)
      : m_response_serializer(response_serializer),
        m_socket(subscribe_socket){};

  // Initialize connection.
  void init(const std::string &endpoint, const std::string &topic);

  // Get the file descriptor of the socket.
  int get_fd();

  // Receive a message from the server. Store the result in "msg".
  error_t receive(TResponseMsg &msg);

protected:
  Serializer<TResponseMsg> &m_response_serializer;
  TSocket &m_socket;
};

// An implementation of BaseRequestClient with zmqpp socket as the socket used
// and Google Protobuf as the serializer.
template <class TRequestMsg, class TResponseMsg>
class RequestClient
    : public BaseRequestClient<TRequestMsg, TResponseMsg, zmqpp::socket> {
public:
  RequestClient(const std::string &endpoint)
      : m_request_serializer(), m_response_serializer(), m_context(),
        m_socket(m_context, zmqpp::socket_type::req),
        BaseRequestClient<TRequestMsg, TResponseMsg, zmqpp::socket>(
            m_request_serializer, m_response_serializer, m_socket) {
    this->init(endpoint);
  };

protected:
  PbSerializer<TRequestMsg> m_request_serializer;
  PbSerializer<TResponseMsg> m_response_serializer;
  zmqpp::context m_context;
  zmqpp::socket m_socket;
};

// An implementation of BaseSubscriber with zmqpp socket as the socket used
// and Google Protobuf as the serializer.
template <class TResponseMsg>
class Subscriber : public BaseSubscriber<TResponseMsg, zmqpp::socket> {
public:
  Subscriber(const std::string &endpoint, const std::string &topic)
      : m_response_serializer(), m_context(),
        m_socket(m_context, zmqpp::socket_type::sub),
        BaseSubscriber<TResponseMsg, zmqpp::socket>(m_response_serializer,
                                                    m_socket) {
    this->init(endpoint, topic);
  };

protected:
  PbSerializer<TResponseMsg> m_response_serializer;
  zmqpp::context m_context;
  zmqpp::socket m_socket;
};

} // namespace daemon_manager

#include "daemon_client.ipp"
#endif /* __DAEMON_CLIENT_H__ */
