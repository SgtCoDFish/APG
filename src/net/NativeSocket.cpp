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

/*
 * Inspired by SDL2_net.
 * See http://hg.libsdl.org/SDL_net/file/c7fa72f19b14/SDLnet.c
 * And https://msdn.microsoft.com/en-us/library/windows/desktop/ms737629(v=vs.85).aspx for Windows.
 *
 * Also uses ideas from beej's guide:
 * http://beej.us/guide
 */

#ifndef APG_NO_NATIVE

#include <cstdio>
#include <cstring>
#include <cerrno>

#include <chrono>

#include "APG/net/NativeSocket.hpp"
#include "APG/core/APGeasylogging.hpp"

namespace APG {

NativeSocket::NativeSocket(const char *remoteHost, uint16_t port, bool autoConnect, uint32_t bufferSize) :
		        Socket(remoteHost, port, bufferSize),
		        portString { std::to_string(port) } {
	if (autoConnect) {
		this->connect();
	}
}

NativeSocket::NativeSocket(int fd, const std::string &remoteHost, uint16_t port, uint32_t bufferSize) :
		        Socket(remoteHost, port, bufferSize),
		        portString { std::to_string(port) },
		        internalSocket { fd },
		        connected { true } {
	addToSet();
}

std::unique_ptr<Socket> NativeSocket::fromRawFileDescriptor(int fd, sockaddr_storage addr) {
	// TODO: Make this more thread-friendly
	char ip[INET6_ADDRSTRLEN];
	int port = -1;

	if (addr.ss_family == AF_INET) {
		sockaddr_in *s = (sockaddr_in *) &addr;

		port = ntohs(s->sin_port);
		if (inet_ntop(AF_INET, &s->sin_addr, ip, INET6_ADDRSTRLEN) == nullptr) {
			std::strcpy(ip, "UNKNOWN4");
		}
	} else if (addr.ss_family == AF_INET6) {
		sockaddr_in6 *s = (sockaddr_in6 *) &addr;

		port = ntohs(s->sin6_port);
		if (inet_ntop(AF_INET6, &s->sin6_addr, ip, INET6_ADDRSTRLEN) == nullptr) {
			std::strcpy(ip, "UNKNOWN6");
		}
	}

	el::Loggers::getLogger("APG")->info("Got IP: %v, len: %v", ip, std::strlen(ip));

	return std::make_unique<NativeSocket>(fd, std::string(ip, std::strlen(ip)), port);
}

NativeSocket::~NativeSocket() {
	disconnect();
}

int NativeSocket::send() {
	if (hasError()) {
		el::Loggers::getLogger("APG")->warn("send() called on SDL socket in error state.");
		return 0;
	}

	auto &buf = getBuffer();

	const int total = buf.size();
	int sent = 0;

	while (sent < total) {
		sent += ::send(internalSocket, reinterpret_cast<const char *>(buf.data()) + sent * sizeof(uint8_t), buf.size() - sent, 0);
	}

	if (sent < (int32_t) buf.size()) {
		if (sent >= 0) {
			el::Loggers::getLogger("APG")->warn("Warning: %v bytes sent of %v bytes total.", sent, buf.size());
		} else {
			el::Loggers::getLogger("APG")->error("Send error: %v", NativeSocketUtil::getErrorMessage(errno));
			setError();
			return 0;
		}
	}

	return sent;
}

int NativeSocket::recv(uint32_t length) {
	if (hasError()) {
		el::Loggers::getLogger("APG")->warn("recv() called on SDL socket in error state.");
		return 0;
	}

	auto tempBuffer = std::make_unique<uint8_t[]>(length);
	auto bytesReceived = ::recv(internalSocket, reinterpret_cast<char *>(tempBuffer.get()), length, 0);

	if (bytesReceived <= 0) {
		if (bytesReceived == 0 || errno == EAGAIN || errno == NativeSocketUtil::APGWOULDBLOCK) {
			// remote connection closed/nonblock takes effect
			return 0;
		} else {
			el::Loggers::getLogger("APG")->error("Recv error: %v", NativeSocketUtil::getErrorMessage(errno));
			setError();
			return -1;
		}
	}

	putBytes(tempBuffer.get(), bytesReceived);

	return bytesReceived;
}

bool NativeSocket::waitForActivity(uint32_t millisecondsToWait) {
//	static timeval timeval;
//	timeval.tv_sec = -1;
//	timeval.tv_usec = -1;

// returns the number of FDs in set if successful (i.e. 1 here)
	const auto selRet = ::select(internalSocket + 1, &socketSet, nullptr, nullptr, nullptr);

	if (selRet <= 0) {
		if (selRet == 0) {
			// timeout
			return false;
		} else {
			el::Loggers::getLogger("APG")->error("::select entered an error state: %v", NativeSocketUtil::getErrorMessage(errno));
			setError();
			return false;
		}
	}

	return true;
}

void NativeSocket::connect() {
	const auto logger = el::Loggers::getLogger("APG");
	disconnect();

	addrinfo hints;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	addrinfo * tempAI;

	const int addrRet = ::getaddrinfo(this->remoteHost.c_str(), this->portString.c_str(), &hints, &tempAI);
	if (addrRet != 0) {
		logger->error("Couldn't listen on port %v in NativeAcceptorSocket: %v", port, ::gai_strerror(addrRet));
		setError();
		return;
	}

	auto addrPtr = NativeSocketUtil::make_addrinfo_ptr(tempAI);

	logger->fatal("NativeSocket::connect NYI :(");

	connected = true;
	addToSet();
}

void NativeSocket::disconnect() {
	if (connected) {
		FD_CLR(internalSocket, &socketSet);

		NativeSocketUtil::closeSocket(internalSocket);
	}

	connected = false;
}

void NativeSocket::addToSet() {
	FD_ZERO(&socketSet);

	FD_SET(internalSocket, &socketSet);
}

NativeAcceptorSocket::NativeAcceptorSocket(uint16_t port_, bool autoListen, uint32_t bufferSize_) :
		        AcceptorSocket(port_, bufferSize_),
		        portString { std::to_string(port) } {
	if (autoListen) {
		listen();
	}
}

NativeAcceptorSocket::~NativeAcceptorSocket() {
	disconnect();
}

void NativeAcceptorSocket::disconnect() {
	if (listening) {
		NativeSocketUtil::closeSocket(internalListener);
	}

	listening = false;
}

std::unique_ptr<Socket> NativeAcceptorSocket::acceptSocket(float maxWaitInSeconds) {
	int newFD;
	sockaddr_storage theirAddr;
	socklen_t addrLen;

	if (maxWaitInSeconds > 0.0f) {
		float waitTime = 0.0f;

		auto startTime = std::chrono::high_resolution_clock::now();

		// socket is set to O_NONBLOCK in listen() so -1 can mean
		// errno was set to EWOULDBLOCK and no actual error occurred.
		while ((newFD = ::accept(internalListener, (sockaddr *) &theirAddr, &addrLen)) == -1) {
			if (errno == NativeSocketUtil::APGWOULDBLOCK) {
				auto timeNow = std::chrono::high_resolution_clock::now();
				const float deltaTime =
				        std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count() / 1000.0f;

				startTime = timeNow;

				waitTime += deltaTime;

				if (waitTime > maxWaitInSeconds) {
					return nullptr;
				}
			} else {
				el::Loggers::getLogger("APG")->error("Error in acceptSocket: %v", NativeSocketUtil::getErrorMessage(errno));
				setError();
				return nullptr;
			}
		}
	} else {
		while ((newFD = ::accept(internalListener, (sockaddr *) &theirAddr, &addrLen)) == -1) {
			if (errno == NativeSocketUtil::APGWOULDBLOCK) {
				continue;
			} else {
				el::Loggers::getLogger("APG")->error("Error in acceptSocket: %v", NativeSocketUtil::getErrorMessage(errno));
				setError();
				return nullptr;
			}
		}
	}

	return NativeSocket::fromRawFileDescriptor(newFD, theirAddr);
}

std::unique_ptr<Socket> NativeAcceptorSocket::acceptSocketOnce() {
	int newFD;
	sockaddr_storage theirAddr;
	socklen_t addrLen;

	if ((newFD = ::accept(internalListener, (sockaddr *) &theirAddr, &addrLen)) == -1) {
		// We almost expect that it would have blocked, but certainly it's not an error.
		if (errno != NativeSocketUtil::APGWOULDBLOCK) {
			el::Loggers::getLogger("APG")->error("Error in acceptSocket: %v", NativeSocketUtil::getErrorMessage(errno));
			setError();
		}
		
		return nullptr;
	}

	return NativeSocket::fromRawFileDescriptor(newFD, theirAddr);
}

void NativeAcceptorSocket::listen() {
	const auto logger = el::Loggers::getLogger("APG");
	disconnect();

	// TODO: IPv6 here?
	addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	addrinfo * retInfoTemp_;

	const int addrRet = ::getaddrinfo(nullptr, portString.c_str(), &hints, &retInfoTemp_);

	auto retHolder = NativeSocketUtil::make_addrinfo_ptr(retInfoTemp_);

	if (addrRet != 0) {
		logger->error("Couldn't listen on port %v in NativeAcceptorSocket: %v", port, ::gai_strerror(addrRet));
		setError();
		return;
	}

	internalListener = NativeSocketUtil::findValidSocket(retHolder, nullptr, true);

	if (internalListener == -1) {
		logger->error("Couldn't find a valid socket to listen on: %v", NativeSocketUtil::getErrorMessage(errno));
		setError();
		return;
	}
	
	if (NativeSocketUtil::setNonBlocking(internalListener) != 0) {
		logger->error("Couldn't set non-blocking state on listening socket: %v", NativeSocketUtil::getErrorMessage(errno));
		setError();
		return;
	}

	if (::listen(internalListener, NativeAcceptorSocket::CONNECTION_BACKLOG_SIZE) == -1) {
		logger->error("Couldn't complete ::listen: %v", NativeSocketUtil::getErrorMessage(errno));
		setError();
		return;
	}

	listening = true;
}

NativeSocketUtil::addrinfo_ptr NativeSocketUtil::make_addrinfo_ptr(addrinfo * ainfo) {
	return NativeSocketUtil::addrinfo_ptr(ainfo, freeaddrinfo);
}

int NativeSocketUtil::findValidSocket(const addrinfo_ptr &ptr, addrinfo **targetStruct, bool shouldBind) {
	auto logger = el::Loggers::getLogger("APG");

	int socketFD = -1;

	addrinfo *p;

	for (p = ptr.get(); p != nullptr; p = p->ai_next) {
		socketFD = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol);

		if (socketFD == -1) {
			logger->verbose(9, "Couldn't establish socket connection for addrinfo struct.");
			continue;
		}

		if (shouldBind) {
			if (::bind(socketFD, p->ai_addr, p->ai_addrlen) == -1) {
				NativeSocketUtil::closeSocket(socketFD);
				logger->error("Couldn't bind socket.");
				return -1;
			} else {
				break;
			}
		} else {
			if (::connect(socketFD, p->ai_addr, p->ai_addrlen) == -1) {
				NativeSocketUtil::closeSocket(socketFD);
				logger->error("Couldn't connect socket.");
				return -1;
			} else {
				break;
			}
		}
	}

	if (targetStruct != nullptr) {
		targetStruct = &p;
	}

	return socketFD;
}

int NativeSocketUtil::closeSocket(int socketFD) {
#ifdef _WIN32
	return ::closesocket(socketFD);
#else
	return ::close(socketFD);
#endif
}

int NativeSocketUtil::setNonBlocking(int socketFD) {
#ifdef _WIN32
	static u_long NON_BLOCK = 1UL; 
	return ::ioctlsocket(socketFD, FIONBIO, &NON_BLOCK);
#else
	return ::fcntl(socketFD, F_SETFL, O_NONBLOCK);
#endif	
}

void NativeSocket::nativeSocketInit() {
#ifdef _WIN32
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if(result != 0) {
		el::Loggers::getLogger("APG")->fatal("Couldn't initialise winsock2.");
		return;
	} else {
		el::Loggers::getLogger("APG")->verbose(9, "Initialised winsock2.");
	}
#else
	/* No-op on non-windows platforms */
#endif
}

void NativeSocket::nativeSocketCleanup() {
#ifdef _WIN32
// Taken from SDL2_net
// http://hg.libsdl.org/SDL_net/file/c7fa72f19b14/SDLnet.c#l118
	if(WSACleanup() == SOCKET_ERROR) {
		if(WSAGetLastError() == WSAEINPROGRESS) {
			WSACancelBlockingCall();
			WSACleanup();
		}
	}
#else
	/* No-op on non-windows platforms */
#endif
}

std::string NativeSocketUtil::getErrorMessage(int errorCode) {
#ifdef _WIN32
	static constexpr const int ERR_BUF_SIZE = 256;
	char charBuf[ERR_BUF_SIZE];
	
	const int retCount = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,//
					nullptr, //
					errorCode,
					0, //
					charBuf, //
					ERR_BUF_SIZE, //
					nullptr
					);
		
	return std::string(charBuf, retCount);
#else
	return std::string(std::strerror(errorCode));
#endif
}

}

#endif
