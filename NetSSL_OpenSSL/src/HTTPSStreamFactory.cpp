//
// HTTPSStreamFactory.cpp
//
// $Id: //poco/Main/NetSSL_OpenSSL/src/HTTPSStreamFactory.cpp#8 $
//
// Library: NetSSL_OpenSSL
// Package: HTTPSClient
// Module:  HTTPSStreamFactory
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


#include "Poco/Net/HTTPSStreamFactory.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPIOStream.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/URI.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/UnbufferedStreamBuf.h"


using Poco::URIStreamFactory;
using Poco::URI;
using Poco::URIStreamOpener;
using Poco::UnbufferedStreamBuf;


namespace Poco {
namespace Net {


HTTPSStreamFactory::HTTPSStreamFactory():
	_proxyPort(HTTPSession::HTTP_PORT)
{
}


HTTPSStreamFactory::HTTPSStreamFactory(const std::string& proxyHost, Poco::UInt16 proxyPort):
	_proxyHost(proxyHost),
	_proxyPort(proxyPort)
{
}


HTTPSStreamFactory::~HTTPSStreamFactory()
{
}


std::istream* HTTPSStreamFactory::open(const URI& uri)
{
	poco_assert (uri.getScheme() == "https");

	URI resolvedURI(uri);
	HTTPClientSession* pSession = 0;
	try
	{
		int redirects = 0;
		do
		{
			pSession = new HTTPSClientSession(resolvedURI.getHost(), resolvedURI.getPort());
			pSession->setProxy(_proxyHost, _proxyPort);
			std::string path = resolvedURI.getPathAndQuery();
			if (path.empty()) path = "/";
			HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
			pSession->sendRequest(req);
			HTTPResponse res;
			std::istream& rs = pSession->receiveResponse(res);
			bool moved = (res.getStatus() == HTTPResponse::HTTP_MOVED_PERMANENTLY || 
			              res.getStatus() == HTTPResponse::HTTP_FOUND || 
			              res.getStatus() == HTTPResponse::HTTP_SEE_OTHER);
			if (moved)
			{
				resolvedURI.resolve(res.get("Location"));
				delete pSession;
				++redirects;
			}
			else if (res.getStatus() == HTTPResponse::HTTP_OK)
			{
				return new HTTPResponseStream(rs, pSession);
			}
			else throw HTTPException(res.getReason(), uri.toString());
		}
		while (redirects < MAX_REDIRECTS);
		throw HTTPException("Too many redirects", uri.toString());
	}
	catch (...)
	{
		delete pSession;
		throw;
	}
}


void HTTPSStreamFactory::registerFactory()
{
	std::string https("https");
	URIStreamOpener::defaultOpener().registerStreamFactory(https, new HTTPSStreamFactory);
}


} } // namespace Poco::Net