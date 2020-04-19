#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "daemon_client.hpp"
#include "daemon_server.hpp"

using namespace daemon_manager;

namespace py = pybind11;

class PyPbSerializer : public Serializer<py::object> {
  using Serializer<py::object>::Serializer;

public:
  PyPbSerializer(py::object cls) : m_cls(cls) {}
  bool encode(py::object const &msg, std::string &data) override {
    data = (py::bytes)msg.attr("SerializeToString")();
    return true;
  }

  bool decode(std::string const &data, py::object &msg) override {
    msg = m_cls();
    py::bytes pydata(data);
    msg.attr("ParseFromString")(pydata);
    return true;
  }

  int get_cmd_tag(py::object const &request) override {
    py::list fields = request.attr("ListFields")();
    py::tuple cmd_metadata = fields[0];
    return cmd_metadata[0].attr("number").cast<int>();
  }

  py::object m_cls;
};

class PyServer : public BaseServer<py::object, py::object, zmqpp::socket> {
  using BaseServer<py::object, py::object, zmqpp::socket>::BaseServer;

public:
  PyServer(py::object request_cls, py::object response_cls,
           const std::string &request_endpoint = "",
           const std::string &subscribe_endpoint = "")
      : m_context(), m_request_socket(m_context, zmqpp::socket_type::rep),
        m_publish_socket(m_context, zmqpp::socket_type::pub),
        m_request_serializer(request_cls), m_response_serializer(response_cls),
        BaseServer(m_request_serializer, m_response_serializer,
                   m_request_socket, m_publish_socket) {
    this->init(request_endpoint, subscribe_endpoint);
  }

private:
  PyPbSerializer m_request_serializer;
  PyPbSerializer m_response_serializer;
  zmqpp::context m_context;
  zmqpp::socket m_request_socket;
  zmqpp::socket m_publish_socket;
};

class PyRequestClient
    : public BaseRequestClient<py::object, py::object, zmqpp::socket> {
  using BaseRequestClient<py::object, py::object,
                          zmqpp::socket>::BaseRequestClient;

public:
  PyRequestClient(py::object request_cls, py::object response_cls,
                  const std::string &endpoint)
      : m_request_serializer(request_cls), m_response_serializer(response_cls),
        m_context(), m_socket(m_context, zmqpp::socket_type::req),
        BaseRequestClient<py::object, py::object, zmqpp::socket>(
            m_request_serializer, m_response_serializer, m_socket) {
    this->init(endpoint);
  };

  py::object pyrequest(const py::object &request) {
    py::object response = m_response_serializer.m_cls();
    error_t err = this->request(request, response);
    if (err) {
      throw std::runtime_error("failed sending request to target, errno - " +
                               std::to_string(err));
    }
    return response;
  }

private:
  PyPbSerializer m_request_serializer;
  PyPbSerializer m_response_serializer;
  zmqpp::context m_context;
  zmqpp::socket m_socket;
};

class PySubscriber : public BaseSubscriber<py::object, zmqpp::socket> {
  using BaseSubscriber<py::object, zmqpp::socket>::BaseSubscriber;

public:
  PySubscriber(py::object response_cls, const std::string &endpoint,
               const std::string &topic)
      : m_response_serializer(response_cls), m_context(),
        m_socket(m_context, zmqpp::socket_type::sub),
        BaseSubscriber<py::object, zmqpp::socket>(m_response_serializer,
                                                  m_socket) {
    this->init(endpoint, topic);
  };

  py::object pyreceive() {
    py::object response = m_response_serializer.m_cls();
    error_t err = this->receive(response);
    if (err) {
      throw std::runtime_error("bad message received from publisher");
    }
    return response;
  }

private:
  PyPbSerializer m_response_serializer;
  zmqpp::context m_context;
  zmqpp::socket m_socket;
};

PYBIND11_MODULE(pydaemon, m) {
  py::class_<PyServer>(m, "daemon_server")
      .def(py::init<py::object, py::object, const std::string &,
                    const std::string &>(),
           py::arg("request_cls"), py::arg("response_cls"),
           py::arg("request_endpoint") = "", py::arg("subscribe_endpoint") = "")
      .def("register_callback", &PyServer::register_callback)
      .def("publish", &PyServer::publish)
      .def("get_fd", &PyServer::get_fd)
      .def("handle_request", &PyServer::handle_request);
  py::class_<PyRequestClient>(m, "daemon_request_client")
      .def(py::init<py::object, py::object, const std::string &>(),
           py::arg("request_cls"), py::arg("response_cls"), py::arg("endpoint"))
      .def("request", &PyRequestClient::pyrequest);
  py::class_<PySubscriber>(m, "daemon_subscriber")
      .def(py::init<py::object, const std::string &, const std::string &>(),
           py::arg("response_cls"), py::arg("endpoint"), py::arg("topic") = "")
      .def("get_fd", &PySubscriber::get_fd)
      .def("receive", &PySubscriber::pyreceive);
}
