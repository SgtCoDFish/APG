#ifndef APG_NO_SDL

#include <memory>
#include <utility>
#include <chrono>
#include <array>

#include "APG/net/SDLSocket.hpp"

#include "APG/internal/Assert.hpp"
#include "APG/net/Socket.hpp"

namespace APG {

SDLSocket::SDLSocket(const std::string &remoteHost_, uint16_t port_, bool autoConnect, uint32_t bufferSize_) :
		        Socket(remoteHost_, port_, bufferSize_),
				logger {spdlog::get("APG")} {
	if (autoConnect) {
		connect();
	}
}

SDLSocket::SDLSocket(TCPsocket socket_, IPaddress *ip_, const char *remoteHost_, uint16_t port_) :
		        Socket(remoteHost_, port_),
				logger {spdlog::get("APG")} {
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
	if (isConnected()) {
		SDLNet_TCP_Close(internalSocket);
	}

	setNotConnected();
}

int SDLSocket::send() {
	if (hasError()) {
		logger->warn("send() called on SDL socket in error state.");
		return 0;
	}

	auto &buf = getBuffer();

	const auto sent = SDLNet_TCP_Send(internalSocket, buf.data(), size());

	if (sent < static_cast<int32_t>(size())) {
		logger->error("Send error: {}", SDLNet_GetError());
		setError();
		return 0;
	}

#ifndef APG_SOCKET_NO_AUTO_CLEAR
	clear();
#endif

	return sent;
}

int SDLSocket::recv(uint32_t length) {
	REQUIRE(length <= APG_RECV_BUFFER_SIZE, "Cannot recv() on a buffer size smaller than the max");

	if (hasError()) {
		logger->warn("recv() called on SDL socket in error state.");
		return 0;
	}

#ifndef APG_SOCKET_NO_AUTO_CLEAR
	clear();
#endif

	auto received = SDLNet_TCP_Recv(internalSocket, recvBuffer.get(), length);

	if (received <= 0) {
		logger->error("Couldn't read data: {}", SDLNet_GetError());
		setError();
		return 0;
	}

	putBytes(recvBuffer.get(), received);

	return received;
}

bool SDLSocket::hasActivity() {
	const auto active = SDLNet_CheckSockets(socketSet.get(), 0);

	if (active == -1) {
		logger->error("SDLNet_CheckSockets returned an error code in waitForActivity: {}",
		        SDLNet_GetError());
		setError();
		return false;
	}

	return active > 0;
}

bool SDLSocket::waitForActivity(uint32_t millisecondsToWait) {
	const int active = SDLNet_CheckSockets(socketSet.get(), millisecondsToWait);

	if (active == -1) {
		logger->error("SDLNet_CheckSockets returned an error code in waitForActivity: {}",
		        SDLNet_GetError());
		setError();
		return false;
	}

	return active > 0;
}

void SDLSocket::connect() {
	disconnect();

	if (SDLNet_ResolveHost(&internalIP, remoteHost.c_str(), port) != 0) {
		logger->error("Couldn't resolve host for socket for hostname \"{}\" on port {}.",
		        remoteHost, port);
		logger->error("SDLNet Error: {}", SDLNet_GetError());
		setError();
		return;
	}

	internalSocket = SDLNet_TCP_Open(&internalIP);

	if (internalSocket == nullptr) {
		logger->error("Couldn't open send socket for hostname \"{}\" on port {}.", remoteHost,
		        port);
		logger->error("SDLNet Error: {}", SDLNet_GetError());
		setError();
		return;
	}

	setConnected();

	addToSet();
}

void SDLSocket::addToSet() {
	socketSet = SXXDL::net::make_socket_set_ptr(SDLNet_AllocSocketSet(1));

	SDLNet_TCP_AddSocket(socketSet.get(), internalSocket);
}

SDLAcceptorSocket::SDLAcceptorSocket(uint16_t port_, bool autoListen, uint32_t bufferSize) :
		        AcceptorSocket(port_, bufferSize) {
	if (autoListen) {
		listen();
	}
}

SDLAcceptorSocket::~SDLAcceptorSocket() {
	disconnect();
}

void SDLAcceptorSocket::disconnect() {
	if (isConnected()) {
		SDLNet_TCP_Close(internalAcceptor);
	}

	setNotConnected();
}

void SDLAcceptorSocket::listen() {
	disconnect();

	if (SDLNet_ResolveHost(&internalIP, nullptr, port) != 0) {
		logger->error("Couldn't resolve host for acceptor on port {}.", port);
		logger->error("SDLNet Error: {}", SDLNet_GetError());
		setError();
		return;
	}

	internalAcceptor = SDLNet_TCP_Open(&internalIP);

	if (internalAcceptor == nullptr) {
		logger->error("Couldn't open server (acceptor) socket on port {}.", port);
		logger->error("SDLNet Error: {}", SDLNet_GetError());
		setError();
		return;
	}

	setConnected();
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
