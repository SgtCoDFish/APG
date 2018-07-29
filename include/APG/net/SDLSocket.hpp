#ifndef INCLUDE_APG_NET_SDLSOCKET_HPP_
#define INCLUDE_APG_NET_SDLSOCKET_HPP_

#ifndef APG_NO_SDL

#include <cstdint>

#include <string>
#include <memory>

#include "spdlog/spdlog.h"

#include "APG/SDL.hpp"

#include "APG/SXXDL.hpp"

#include "Socket.hpp"
#include "ByteBuffer.hpp"

namespace APG {

class SDLSocket : public Socket {
public:
	static std::unique_ptr<SDLSocket> fromRawSDLSocket(TCPsocket socket);

	explicit SDLSocket(const std::string &hostName, uint16_t port, bool autoConnect = false, uint32_t bufferSize = BB_DEFAULT_SIZE);
	/**
	 * Not recommended for general use, mainly used by unique_ptr
	 */
	explicit SDLSocket(TCPsocket readSocket_, IPaddress *ip_, const char *remoteHost_, uint16_t port_);
	virtual ~SDLSocket();

	virtual int send() override final;
	virtual int recv(uint32_t length = 1024u) override final;

	virtual bool hasActivity() override final;
	virtual bool waitForActivity(uint32_t millisecondsToWait = 0u) override final;

	const TCPsocket &getSDLSocket() const {
		return internalSocket;
	}

	virtual void connect() override final;
	virtual void disconnect() override final;

private:
	void addToSet();

	TCPsocket internalSocket;
	SXXDL::net::socket_set_ptr socketSet = SXXDL::net::make_socket_set_ptr(nullptr);

	IPaddress internalIP;

	std::shared_ptr<spdlog::logger> logger;
};

class SDLAcceptorSocket final : public AcceptorSocket {
public:
	explicit SDLAcceptorSocket(uint16_t port, bool autoListen = false, uint32_t bufferSize = BB_DEFAULT_SIZE);
	virtual ~SDLAcceptorSocket();

	virtual std::unique_ptr<Socket> acceptSocket(float maxWaitInSeconds = -1.0f) override final;
	virtual std::unique_ptr<Socket> acceptSocketOnce() override final;

	const TCPsocket &getSDLAcceptor() const {
		return internalAcceptor;
	}

	/**
	 * Only disconnects the acceptor socket so no new connections can be established.
	 * Sockets created by acceptSocket* are not affected.
	 */
	virtual void disconnect() override final;

protected:
	virtual void listen() override final;

private:
	TCPsocket internalAcceptor;
	IPaddress internalIP;

	float waitTime = 0.0f;

	std::shared_ptr<spdlog::logger> logger;
};

}

#endif

#endif