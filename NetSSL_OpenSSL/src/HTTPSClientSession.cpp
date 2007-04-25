//
// HTTPSClientSession.cpp
//
// $Id: //poco/Main/NetSSL_OpenSSL/src/HTTPSClientSession.cpp#11 $
//
// Library: NetSSL_OpenSSL
// Package: HTTPSClient
// Module:  HTTPSClientSession
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


#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/SecureStreamSocket.h"
#include "Poco/Net/SecureStreamSocketImpl.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPHeaderStream.h"
#include "Poco/Net/HTTPStream.h"
#include "Poco/Net/HTTPFixedLengthStream.h"
#include "Poco/Net/HTTPChunkedStream.h"
#include "Poco/Net/NetException.h"
#include "Poco/NumberFormatter.h"


using Poco::NumberFormatter;
using Poco::IllegalStateException;


namespace Poco {
namespace Net {


HTTPSClientSession::HTTPSClientSession():
	HTTPClientSession(SecureStreamSocket())
{
	setPort(Utility::HTTPS_PORT);
}


HTTPSClientSession::HTTPSClientSession(const SecureStreamSocket& socket):
	HTTPClientSession(socket)
{
	setPort(Utility::HTTPS_PORT);
}


HTTPSClientSession::HTTPSClientSession(const std::string& host, Poco::UInt16 port):
	HTTPClientSession(SecureStreamSocket())
{
	setHost(host);
	setPort(port);
}


HTTPSClientSession::~HTTPSClientSession()
{
}


std::ostream& HTTPSClientSession::sendRequest(HTTPRequest& request)
{
	deleteResponseStream();

	bool keepAlive = getKeepAlive();
	if (connected() && !keepAlive)
		close();
	if (!connected())
		reconnect();
	if (!keepAlive)
		request.setKeepAlive(false);
	request.setHost(getHost(), getPort());
	
	{
		HTTPHeaderOutputStream hos(*this);
		setReconnect(keepAlive);
		request.write(hos);
		setReconnect(false);
		setExpectResponseBody(request.getMethod() != HTTPRequest::HTTP_HEAD);
	}
	if (request.getChunkedTransferEncoding())
		setRequestStream(new HTTPChunkedOutputStream(*this));
	else if (request.getContentLength() != HTTPMessage::UNKNOWN_CONTENT_LENGTH)
		setRequestStream(new HTTPFixedLengthOutputStream(*this, request.getContentLength()));
	else if (request.getMethod() == HTTPRequest::HTTP_GET || request.getMethod() == HTTPRequest::HTTP_HEAD)
		setRequestStream(new HTTPFixedLengthOutputStream(*this, 0));
	else
		setRequestStream(new HTTPOutputStream(*this));

	return *getRequestStream();
}


std::istream& HTTPSClientSession::receiveResponse(HTTPResponse& response)
{
	deleteRequestStream();

	do
	{
		response.clear();
		HTTPHeaderInputStream his(*this);
		try
		{
			response.read(his);
		}
		catch (MessageException&)
		{
			if (networkException())
				networkException()->rethrow();
			else
				throw;
		}
	}
	while (response.getStatus() == HTTPResponse::HTTP_CONTINUE);

	if (!getExpectResponseBody())
		setResponseStream(new HTTPFixedLengthInputStream(*this, 0));
	else if (response.getChunkedTransferEncoding())
		setResponseStream(new HTTPChunkedInputStream(*this));
	else if (response.getContentLength() != HTTPMessage::UNKNOWN_CONTENT_LENGTH)
		setResponseStream(new HTTPFixedLengthInputStream(*this, response.getContentLength()));
	else
		setResponseStream(new HTTPInputStream(*this));
		
	return *getResponseStream();
}


std::string HTTPSClientSession::getHostInfo() const
{
	std::string result("https://");
	result.append(getHost());
	result.append(":");
	result.append(NumberFormatter::format(getPort()));
	return result;
}


void HTTPSClientSession::connect(const SocketAddress& address)
{
	if (!getProxyHost().empty())
	{
		StreamSocket& aSock = socket();
		SecureStreamSocketImpl* pImplSock = dynamic_cast<SecureStreamSocketImpl*>(aSock.impl());
		poco_check_ptr (pImplSock);
		pImplSock->setTunnelEndPoint(getHost(), getPort());
	}
	HTTPSession::connect(address);
}


} } // namespace Poco::Net