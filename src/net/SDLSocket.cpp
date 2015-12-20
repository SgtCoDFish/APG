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

#include "APG/net/SDLSocket.hpp"
#include "APG/core/APGeasylogging.hpp"

#include <memory>
#include <utility>
#include <chrono>
#include <array>

namespace APG {

SDLSocket::SDLSocket(const char *remoteHost_, uint16_t port_, bool autoConnect) :
		        Socket(remoteHost_, port_) {
	if (autoConnect) {
		connect();
	}
}

SDLSocket::SDLSocket(TCPsocket readSocket_, IPaddress *ip_, const char *remoteHost_, uint16_t port_,
        TCPsocket sendSocket_) :
		        Socket(remoteHost_, port_),
		        readConnected { true },
		        sendConnected { true } {
	internalReadSocket = readSocket_;
	internalSendSocket = sendSocket_;

	internalIP = *ip_;
}

std::unique_ptr<SDLSocket> SDLSocket::fromRawSDLSockets(TCPsocket readSocket, TCPsocket sendSocket) {
	auto addr = SDLNet_TCP_GetPeerAddress(readSocket);
	const char * hostname = SDLNet_ResolveIP(addr);

	return std::make_unique<SDLSocket>(readSocket, addr, hostname, addr->port, sendSocket);
}

SDLSocket::~SDLSocket() {
	disconnect();
}

void SDLSocket::disconnect() {
	if (readConnected) {
		SDLNet_TCP_Close(internalReadSocket);
	}

	if (sendConnected) {
		SDLNet_TCP_Close(internalSendSocket);
	}
}

int SDLSocket::send() {
	if (hasError()) {
		el::Loggers::getLogger("APG")->warn("send() called on SDL socket in error state.");
		return 0;
	}

	auto &buffer = this->getBuffer();

	auto sent = SDLNet_TCP_Send(internalSendSocket, buffer.data(), buffer.size());

	if (sent < (int32_t) buffer.size()) {
		el::Loggers::getLogger("APG")->error("Send error: %v", SDLNet_GetError());
		setError();
	}

	return sent;
}

int SDLSocket::recv(uint32_t length) {
	if (hasError()) {
		el::Loggers::getLogger("APG")->warn("recv() called on SDL socket in error state.");
		return 0;
	}

	auto tempSendBuffer = std::make_unique<uint8_t[]>(length);
	auto received = SDLNet_TCP_Recv(internalReadSocket, tempSendBuffer.get(), length);

	if (received <= 0) {
		el::Loggers::getLogger("APG")->error("Couldn't read data: %v", SDLNet_GetError());
		setError();
	}

	putBytes(tempSendBuffer.get(), received);

	return received;
}

void SDLSocket::connect() {
	disconnect();

	if (SDLNet_ResolveHost(&internalIP, remoteHost, port) != 0) {
		el::Loggers::getLogger("APG")->error("Couldn't resolve host for socket for hostname \"%v\" on port %v.",
		        remoteHost, port);
		el::Loggers::getLogger("APG")->error("SDLNet Error: %v", SDLNet_GetError());
		setError();
		return;
	}

	internalSendSocket = SDLNet_TCP_Open(&internalIP);

	if (internalSendSocket == nullptr) {
		el::Loggers::getLogger("APG")->error("Couldn't open send socket for hostname \"%v\" on port %v.", remoteHost,
		        port);
		el::Loggers::getLogger("APG")->error("SDLNet Error: %v", SDLNet_GetError());
		setError();
		return;
	}

	sendConnected = true;

	static const uint16_t port = 10666;
	IPaddress acceptorIP;
	// TODO remove hardcoded port
	if (SDLNet_ResolveHost(&acceptorIP, nullptr, port) != 0) {
		el::Loggers::getLogger("APG")->error("Couldn't open port %v for creating a read socket.", port);
		setError();
		return;
	}

	TCPsocket acceptorSocket = SDLNet_TCP_Open(&acceptorIP);

	if (acceptorSocket == nullptr) {
		el::Loggers::getLogger("APG")->error(
		        "Couldn't open server (acceptor) socket on port %v to establish a readSocket.", port);
		el::Loggers::getLogger("APG")->error("SDLNet Error: %v", SDLNet_GetError());
		setError();
		return;
	}

	auto startTime = std::chrono::high_resolution_clock::now();
	while (true) {
		internalReadSocket = SDLNet_TCP_Accept(acceptorSocket);

		if (internalReadSocket != nullptr) {
			break;
		}

		auto timeNow = std::chrono::high_resolution_clock::now();
		const float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count()
		        / 1000.0f;

		if (deltaTime > 5) {
			el::Loggers::getLogger("APG")->error("Timeout when connecting readSocket.");

			setError();
			return;
		}
	}

	if (acceptorSocket != nullptr) {
		SDLNet_TCP_Close(acceptorSocket);
	}

	readConnected = true;
}

SDLAcceptorSocket::SDLAcceptorSocket(uint16_t port_, bool autoListen) :
		        AcceptorSocket(port_) {
	if (autoListen) {
		listen();
	}
}

SDLAcceptorSocket::~SDLAcceptorSocket() {
	disconnect();
}

void SDLAcceptorSocket::disconnect() {
	if (acceptorConnected) {
		SDLNet_TCP_Close(internalAcceptor);
	}
}

void SDLAcceptorSocket::listen() {
	disconnect();

	if (SDLNet_ResolveHost(&internalIP, nullptr, port) != 0) {
		el::Loggers::getLogger("APG")->error("Couldn't resolve host for acceptor on port %v.", port);
		el::Loggers::getLogger("APG")->error("SDLNet Error: %v", SDLNet_GetError());
		setError();
		return;
	}

	internalAcceptor = SDLNet_TCP_Open(&internalIP);

	if (internalAcceptor == nullptr) {
		el::Loggers::getLogger("APG")->error("Couldn't open server (acceptor) socket on port %v.", port);
		el::Loggers::getLogger("APG")->error("SDLNet Error: %v", SDLNet_GetError());
		setError();
		return;
	}

	acceptorConnected = true;
}

std::unique_ptr<Socket> SDLAcceptorSocket::acceptSocket(float maxWaitInSeconds) {
	TCPsocket readSocket;

	if (maxWaitInSeconds > 0.0f) {
		waitTime = 0.0f;

		auto startTime = std::chrono::high_resolution_clock::now();

		while ((readSocket = SDLNet_TCP_Accept(internalAcceptor)) == nullptr) {
			auto timeNow = std::chrono::high_resolution_clock::now();
			const float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - startTime).count()
			        / 1000.0f;

			startTime = timeNow;

			waitTime += deltaTime;

			if (waitTime > maxWaitInSeconds) {
				return nullptr;
			}
		}
	} else {
		while ((readSocket = SDLNet_TCP_Accept(internalAcceptor)) == nullptr) {
		}
	}

	IPaddress * remoteAddr = SDLNet_TCP_GetPeerAddress(readSocket);

	// TODO: Because of C, this isn't thread safe. Rewrite, possibly taking code from
	// http://hg.libsdl.org/SDL_net/file/c7fa72f19b14/SDLnet.c#l164
	const char * hostname = SDLNet_ResolveIP(remoteAddr);

	TCPsocket sendSocket;

	IPaddress remoteHost;
	if (SDLNet_ResolveHost(&remoteHost, hostname, 10666) != 0) {
		el::Loggers::getLogger("APG")->error("Couldn't resolve remote host when connecting sendSocket for host %v.",
		        hostname);
		return nullptr;
	}

	sendSocket = SDLNet_TCP_Open(&remoteHost);

	if (sendSocket == nullptr) {
		el::Loggers::getLogger("APG")->info("Couldn't connect send socket: %v.", SDLNet_GetError());
		return nullptr;
	}

	return SDLSocket::fromRawSDLSockets(readSocket, sendSocket);
}

std::unique_ptr<Socket> SDLAcceptorSocket::acceptSocketOnce() {
	TCPsocket readSocket = SDLNet_TCP_Accept(internalAcceptor);

	if (readSocket == nullptr) {
		return nullptr;
	}

	IPaddress * remoteAddr = SDLNet_TCP_GetPeerAddress(readSocket);

	// TODO: Because of C, this isn't thread safe. Rewrite, possibly taking code from
	// http://hg.libsdl.org/SDL_net/file/c7fa72f19b14/SDLnet.c#l164
	const char * hostname = SDLNet_ResolveIP(remoteAddr);

	TCPsocket sendSocket;

	IPaddress remoteHost;
	if (SDLNet_ResolveHost(&remoteHost, hostname, 10666) != 0) {
		el::Loggers::getLogger("APG")->error("Couldn't resolve remote host when connecting sendSocket for host %v.",
		        hostname);
		return nullptr;
	}

	sendSocket = SDLNet_TCP_Open(&remoteHost);

	if (sendSocket == nullptr) {
		el::Loggers::getLogger("APG")->info("Couldn't connect send socket: %v.", SDLNet_GetError());
		return nullptr;
	}

	return SDLSocket::fromRawSDLSockets(readSocket, sendSocket);
}

}
