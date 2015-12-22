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

#ifndef APG_NO_SDL

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

SDLSocket::SDLSocket(TCPsocket socket_, IPaddress *ip_, const char *remoteHost_, uint16_t port_) :
		        Socket(remoteHost_, port_),
		        connected { true } {
	internalSocket = socket_;

	internalIP = *ip_;

	addToSet();
}

std::unique_ptr<SDLSocket> SDLSocket::fromRawSDLSocket(TCPsocket socket) {
	// TODO: Make this thread safe
	auto addr = SDLNet_TCP_GetPeerAddress(socket);
	const char * hostname = SDLNet_ResolveIP(addr);

	return std::make_unique<SDLSocket>(socket, addr, hostname, addr->port);
}

SDLSocket::~SDLSocket() {
	disconnect();
}

void SDLSocket::disconnect() {
	if (connected) {
		SDLNet_TCP_Close(internalSocket);
	}
}

int SDLSocket::send() {
	if (hasError()) {
		el::Loggers::getLogger("APG")->warn("send() called on SDL socket in error state.");
		return 0;
	}

	auto &buffer = this->getBuffer();

	auto sent = SDLNet_TCP_Send(internalSocket, buffer.data(), buffer.size());

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
	auto received = SDLNet_TCP_Recv(internalSocket, tempSendBuffer.get(), length);

	if (received < 0) {
		el::Loggers::getLogger("APG")->error("Couldn't read data: %v", SDLNet_GetError());
		setError();
	}

	putBytes(tempSendBuffer.get(), received);

	return received;
}

bool SDLSocket::waitForActivity(uint32_t millisecondsToWait) {
	const int active = SDLNet_CheckSockets(socketSet.get(), millisecondsToWait);

	if(active == -1) {
		el::Loggers::getLogger("APG")->warn("SDLNet_CheckSockets returned an error code in waitForActivity: %v", SDLNet_GetError());
	}

	return active == 1;
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

	internalSocket = SDLNet_TCP_Open(&internalIP);

	if (internalSocket == nullptr) {
		el::Loggers::getLogger("APG")->error("Couldn't open send socket for hostname \"%v\" on port %v.", remoteHost,
		        port);
		el::Loggers::getLogger("APG")->error("SDLNet Error: %v", SDLNet_GetError());
		setError();
		return;
	}

	connected = true;

	addToSet();
}

void SDLSocket::addToSet() {
	socketSet = SXXDL::net::make_socket_set_ptr(SDLNet_AllocSocketSet(1));

	SDLNet_TCP_AddSocket(socketSet.get(), internalSocket);
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
	TCPsocket socket;

	if (maxWaitInSeconds > 0.0f) {
		waitTime = 0.0f;

		auto startTime = std::chrono::high_resolution_clock::now();

		while ((socket = SDLNet_TCP_Accept(internalAcceptor)) == nullptr) {
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
		while ((socket = SDLNet_TCP_Accept(internalAcceptor)) == nullptr) {
		}
	}

	return SDLSocket::fromRawSDLSocket(socket);
}

std::unique_ptr<Socket> SDLAcceptorSocket::acceptSocketOnce() {
	TCPsocket readSocket = SDLNet_TCP_Accept(internalAcceptor);

	if (readSocket == nullptr) {
		return nullptr;
	}

	return SDLSocket::fromRawSDLSocket(readSocket);
}

}

#endif
