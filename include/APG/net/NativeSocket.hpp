/*
 * Copyright (c) 2015 See AUTHORS file.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef INCLUDE_APG_NET_NATIVESOCKET_HPP_
#define INCLUDE_APG_NET_NATIVESOCKET_HPP_

#ifndef APG_NO_NATIVE

#include <string>
#include <memory>

#ifdef _WIN32
// WIN32_LEAN_AND_MEAN should be defined at build time
// but we need it so we're going to make sure here
#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#include <windows.h>
// this lets us check for things like EWOULDBLOCK
#define errno WSAGetLastError()
#else
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/tcp.h>
#endif

#include "Socket.hpp"
#include "ByteBuffer.hpp"

namespace APG {

class NativeSocketUtil {
public:
	using addrinfo_ptr = std::unique_ptr<addrinfo, void(*)(addrinfo*)>;

	enum class DualSocketReturn {
		IPV4_ONLY,
		IPV6_ONLY,
		BOTH,
		NEITHER
	};

	/**
	 * Traverses a linked list returned by getaddrinfo and calls socket() on each
	 * trying to get a socket descriptor. If it succeeds, returns the descriptor and places
	 * the valid addrinfo into targetStruct.
	 *
	 * Only useful for servers which want to listen on only IP4, only IP6 or IP6-with-mapped-IP4.
	 *
	 * If bind is true, ::bind() will be called on a valid descriptor. If false, ::connect() is called.
	 * @return a valid socket descriptor if a valid addrinfo was found, or -1 on failure.
	 */
	static int findValidSocket(const addrinfo_ptr &ptr, bool bind = false, addrinfo ** targetStruct = nullptr);

	/**
	 * Attempts to fill ip4Socket and ip6Socket with sockets bind()ed sockets
	 * which support both IPv4 and IPv6. Both will listen to the same port.
	 *
	 * Using this function, you should setsockopt for IPV6_V6ONLY or enable this in your OS settings.
	 *
	 * If ip4 or ip6 fails, the corresponding ip4Socket or ip6Socket will be set to mimic the other socket that succeeded,
	 * although there will only be one real socket and it will obviously only support one protocol.
	 *
	 * @return a DualSocketReturn describing what the outcome of the function was.
	 */
	static DualSocketReturn findValidDualSockets(int &ip4Socket, int &ip6Socket, const addrinfo_ptr &ptr,
	        addrinfo **targetStruct4 = nullptr, addrinfo **targetStruct6 = nullptr);

	/**
	 * Close the given socket FD; implemented here to handle differences between Windows and other
	 * platforms.
	 */
	static int closeSocket(int socketFD);

	/**
	 * Sets the given socket to non-blocking mode. Implemented here to mitigate differences
	 * between Windows and other platforms.
	 */
	static int setNonBlocking(int socketFD);

	static int setTCPNodelay(int socketFD);

	static addrinfo_ptr make_addrinfo_ptr(addrinfo *ainfo);

	static std::string getErrorMessage(int errorCode);

#ifdef _WIN32
	static constexpr const int APGWOULDBLOCK = WSAEWOULDBLOCK;
#else
	static constexpr const int APGWOULDBLOCK = EWOULDBLOCK;
#endif
};

class NativeSocket : public Socket {
public:
	/**
	 * Must be called at start of application if native sockets are to be used.
	 *
	 * Mainly for Windows; not calling probably won't be a problem on other platforms.
	 */
	static void nativeSocketInit();

	/**
	 * Must be called at end of application if native sockets are to be used.
	 *
	 * Mainly for Windows.
	 */
	static void nativeSocketCleanup();

	/**
	 * Create a socket from a raw file descriptor.
	 */
	static std::unique_ptr<Socket> fromRawFileDescriptor(int fd, sockaddr_storage theirAddr);

	explicit NativeSocket(const std::string &remoteHost, uint16_t port, bool autoConnect = false, uint32_t bufferSize =
	BB_DEFAULT_SIZE);

	/**
	 * Not reccommended for use; mainly for use by unique_ptr
	 */
	explicit NativeSocket(int fd, const std::string &str, uint16_t port, uint32_t bufferSize = BB_DEFAULT_SIZE);
	virtual ~NativeSocket();

	virtual int send() override final;

	virtual int recv(uint32_t length = 1024u) override final;

	virtual bool hasActivity() override final;

	virtual bool waitForActivity(uint32_t millisecondsToWait = 0u) override final;

	virtual void connect() override final;
	virtual void disconnect() override final;

private:
	void addToSet();
	const std::string portString;

	int internalSocket = -1;

	fd_set socketSet;
};

/**
 * Uses the native socket API (posix sockets, winsock2) on the platform to listen for incoming connections.
 *
 * Will attempt to create 2 sockets listening on the same port; one for IPv4, one for IPv6.
 * If one of those is not available, it can be queried using hasIPXSupport for X = {4, 6}.
 */
class NativeDualAcceptorSocket : public AcceptorSocket {
public:
	explicit NativeDualAcceptorSocket(uint16_t port_, bool autoListen = false, uint32_t bufferSize = BB_DEFAULT_SIZE);
	virtual ~NativeDualAcceptorSocket();

	virtual std::unique_ptr<Socket> acceptSocket(float maxWaitInSeconds = -1.0f) override final;
	virtual std::unique_ptr<Socket> acceptSocketOnce() override final;

	virtual void disconnect() override final;

	bool hasIP4Support() const {
		return supportsIP4;
	}

	bool hasIP6Support() const {
		return supportsIP6;
	}

protected:
	virtual void listen() override final;

private:
	static constexpr const int CONNECTION_BACKLOG_SIZE = 10;

	const std::string portString;

	int internalListener4 = -1;
	int internalListener6 = -1;

	bool supportsIP4 = false;
	bool supportsIP6 = false;
};

}

#endif // notAPG_NO_NATIVE

#endif /* INCLUDE_APG_NET_NATIVESOCKET_HPP_ */
