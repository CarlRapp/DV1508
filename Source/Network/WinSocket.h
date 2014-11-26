#ifdef WIN32
#ifndef WINSOCKET_H
#define WINSOCKET_H
#include <WinSock2.h>

#include "Network/ISocket.h"

class DECLSPEC WinSocket : public ISocket
{

private:
	SOCKET m_socket;
	
	std::string m_remoteIP;
	int m_remotePort;
	int m_localPort;

	static bool m_initialized;

public:
	WinSocket(SOCKET _socket);
	WinSocket(int _domain, int _type, int _protocol);
	~WinSocket();

	static bool Initialize();

	bool Connect(const char* _ip, const int _port);
	bool Bind(const int _port);
	ISocket* Accept(NetConnection& _netConnection);
	bool Listen(int _backlog);
	int Recv(char* _buffer, int _length, int _flags);
	int Send(char* _buffer, int _length, int _flags);

};

#endif
#endif