#ifndef TESTS_MOCK_LIBPATLMS_NETWORK_NETWORK_H
#define TESTS_MOCK_LIBPATLMS_NETWORK_NETWORK_H

#include <gmock/gmock.h>
#include <patlms/network/detail/network_interface.h>

namespace mock
{

namespace libpatlms
{

namespace network
{

class Network : public ::network::detail::NetworkInterface {
 public:
  MOCK_METHOD1(Socket, int(int domain = PF_INET));
  
  MOCK_METHOD1(OpenUnixSocket, int(const std::string &path));
  MOCK_METHOD2(OpenIpv4Socket, int(const std::string &address, int port));
  MOCK_METHOD2(ConnectUnix, void(int socket, const std::string &filesystem_path));
  MOCK_METHOD1(Close, void(int socket));

  MOCK_METHOD2(SendText, void(int socket, const std::string &text));
  MOCK_METHOD1(ReceiveText, const std::string(int socket));

  MOCK_METHOD1(RecvMessage, NetworkMessage(int socket));
  MOCK_METHOD2(SendMessage, void(int socket, const NetworkMessage &message));

  MOCK_METHOD2(WaitForData, WaitStatus(int socket, int timeout));

  MOCK_METHOD1(Accept, ConnectionData(int socket));
};

}

}

}

#endif /* TESTS_MOCK_LIBPATLMS_NETWORK_NETWORK_H */
