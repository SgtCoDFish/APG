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
#ifndef _WIN32

#include <string>
#include <memory>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include "Socket.hpp"
#include "ByteBuffer.hpp"

namespace APG {

class NativeSocketUtil {
public:
	using addrinfo_ptr = std::unique_ptr<addrinfo, void(*)(addrinfo*)>;

	/**
	 * Traverses a linked list returned by getaddrinfo and calls socket() on each
	 * trying to get a socket descriptor. If it succeeds, returns the descriptor and places
	 * the valid addrinfo into targetStruct.
	 *
	 * If bind is true, ::bind() will be called on a valid descriptor. If false, ::connect() is called.
	 * @return a valid socket descriptor if a valid addrinfo was found, or -1 on failure.
	 */
	static int findValidSocket(const addrinfo_ptr &ptr, addrinfo ** targetStruct, bool bind = false);

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

	static addrinfo_ptr make_addrinfo_ptr(addrinfo *ainfo);
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

	explicit NativeSocket(const char * remoteHost, uint16_t port, bool autoConnect = false, uint32_t bufferSize =
	BB_DEFAULT_SIZE);

	/**
	 * Not reccommended for use; mainly for use by unique_ptr
	 */
	explicit NativeSocket(int fd, const std::string &str, uint16_t port, uint32_t bufferSize = BB_DEFAULT_SIZE);
	virtual ~NativeSocket();

	virtual int send() override final;

	virtual int recv(uint32_t length = 1024u) override final;

	virtual bool waitForActivity(uint32_t millisecondsToWait = 0u) override final;

	virtual void connect() override final;
	virtual void disconnect() override final;

private:
	void addToSet();
	const std::string portString;

	int internalSocket = -1;
	bool connected = false;

	fd_set socketSet;
};

class NativeAcceptorSocket : public AcceptorSocket {
public:
	explicit NativeAcceptorSocket(uint16_t port_, bool autoListen = false, uint32_t bufferSize = BB_DEFAULT_SIZE);
	virtual ~NativeAcceptorSocket();

	virtual std::unique_ptr<Socket> acceptSocket(float maxWaitInSeconds = -1.0f) override final;
	virtual std::unique_ptr<Socket> acceptSocketOnce() override final;

	virtual void disconnect() override final;

protected:
	virtual void listen() override final;

private:
	static constexpr const int CONNECTION_BACKLOG_SIZE = 10;

	bool listening = false;
	const std::string portString;

	int internalListener = -1;
};

}

#endif
#endif

#endif /* INCLUDE_APG_NET_NATIVESOCKET_HPP_ */
