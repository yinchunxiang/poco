//
// SecureStreamSocketImpl.cpp
//
// $Id: //poco/Main/NetSSL_OpenSSL/src/SecureStreamSocketImpl.cpp#8 $
//
// Library: NetSSL_OpenSSL
// Package: SSLSockets
// Module:  SecureStreamSocketImpl
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Net/SecureStreamSocketImpl.h"


namespace Poco {
namespace Net {


SecureStreamSocketImpl::SecureStreamSocketImpl()
{
}


SecureStreamSocketImpl::SecureStreamSocketImpl(SSL* _pSSL): _socket(_pSSL)
{
	setSockfd(_socket.sockfd());
}


SecureStreamSocketImpl::~SecureStreamSocketImpl()
{
}


SocketImpl* SecureStreamSocketImpl::acceptConnection(SocketAddress& clientAddr)
{
	return _socket.acceptConnection(clientAddr);
}


void SecureStreamSocketImpl::connect(const SocketAddress& address)
{
	_socket.connect(address);
	setSockfd(_socket.sockfd());
}


void SecureStreamSocketImpl::connect(const SocketAddress& address, const Poco::Timespan& timeout)
{
	_socket.connect(address, timeout);
	setSockfd(_socket.sockfd());
}
	

void SecureStreamSocketImpl::connectNB(const SocketAddress& address)
{
	_socket.connectNB(address);
	setSockfd(_socket.sockfd());
}
	

void SecureStreamSocketImpl::bind(const SocketAddress& address, bool reuseAddress)
{
	_socket.bind(address, reuseAddress);
}

	
void SecureStreamSocketImpl::listen(int backlog)
{
	_socket.listen(backlog);
	setSockfd(_socket.sockfd());
}
	

void SecureStreamSocketImpl::close()
{
	invalidate();
	_socket.close();
}
	

int SecureStreamSocketImpl::sendBytes(const void* buffer, int length, int flags)
{
	const char* p = reinterpret_cast<const char*>(buffer);
	int remaining = length;
	while (remaining > 0)
	{
		int n = _socket.sendBytes(p, length, flags);
		p += n; 
		remaining -= n;
	}
	return length;
}


int SecureStreamSocketImpl::receiveBytes(void* buffer, int length, int flags)
{
	return _socket.receiveBytes(buffer, length, flags);
}


int SecureStreamSocketImpl::sendTo(const void* buffer, int length, const SocketAddress& address, int flags)
{
	return _socket.sendTo(buffer, length, address, flags);
}


int SecureStreamSocketImpl::receiveFrom(void* buffer, int length, SocketAddress& address, int flags)
{
	return _socket.receiveFrom(buffer, length, address, flags);
}


void SecureStreamSocketImpl::sendUrgent(unsigned char data)
{
	return _socket.sendUrgent(data);
}


} } // namespace Poco::Net