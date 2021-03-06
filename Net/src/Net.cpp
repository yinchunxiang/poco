//
// Net.cpp
//
// $Id: //poco/1.4/Net/src/Net.cpp#10 $
//
// Library: Net
// Package: NetCore
// Module:  NetCore
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
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


#include "Poco/Net/Net.h"


#if defined(POCO_OS_FAMILY_WINDOWS)


#include "Poco/Net/SocketDefs.h"
#include "Poco/Net/NetException.h"


namespace Poco {
namespace Net {


void Net_API initializeNetwork()
{
	WORD    version = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(version, &data) != 0)
		throw NetException("Failed to initialize network subsystem");
}


void Net_API uninitializeNetwork()
{
	WSACleanup();
}


} } // namespace Poco::Net


#if !defined(POCO_NET_NO_WINDOWS_INIT)
	#if defined (POCO_DLL)
		BOOL APIENTRY DllMain(HANDLE, DWORD reasonForCall, LPVOID)
		{
			switch(reasonForCall)
			{
			case DLL_PROCESS_ATTACH:
				Poco::Net::initializeNetwork();
				break;
			case DLL_PROCESS_DETACH:
				Poco::Net::uninitializeNetwork();
			}
			return TRUE;
		}
	#else // POCO_STATIC
		struct NetworkInitializer
			/// Network initializer for windows statically
			/// linked library.
		{
			NetworkInitializer()
				/// Calls Poco::Net::initializeNetwork();
			{
				Poco::Net::initializeNetwork();
			}

			~NetworkInitializer()
				/// Calls Poco::Net::uninitializeNetwork();
			{
				Poco::Net::uninitializeNetwork();
			}
		};

		const NetworkInitializer pocoNetworkInitializer;
	#endif // POCO_DLL/POCO_STATIC

#endif // POCO_NET_NO_WINDOWS_INIT


#else // POCO_OS_FAMILY_WINDOWS

void Net_API initializeNetwork()
{
}

void Net_API uninitializeNetwork()
{
}

#endif // POCO_OS_FAMILY_WINDOWS
