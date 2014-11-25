#include "ISocket.h"

#ifdef WIN32
//#include "Network/WinSocket.h"
#else
#include "Network/LinSocket.h"
#endif

static bool Initialize()
{
#ifdef WIN32
	return 0;// WinSocket::Initialize();
#else
	return LinSocket::Initialize();
#endif
}


ISocket* ISocket::CreateISocket(int _domain, int _type, int _protocol)
{
#ifdef WIN32
	return 0;// new WinSocket(_domain, _type, _protocol);
#else
	return new LinSocket(_domain, _type, _protocol);
#endif
}