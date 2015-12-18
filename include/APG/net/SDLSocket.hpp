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


#ifndef INCLUDE_APG_NET_SDLSOCKET_HPP_
#define INCLUDE_APG_NET_SDLSOCKET_HPP_

#include <cstdint>

#include <memory>

#include <SDL2/SDL_net.h>

#include "Socket.hpp"
#include "ByteBuffer.hpp"

namespace APG {

class SDLSocket : public Socket {
public:
	static SDLSocket fromRawSDLSocket(TCPsocket socket);

	explicit SDLSocket(const char * hostName, uint16_t port, bool autoConnect = false);
	virtual ~SDLSocket();

	virtual int send() override final;
	virtual int recv(uint32_t length = 1024u) override final;

	const TCPsocket &getSDLSocket() const {
		return internalSocket;
	}

protected:
	virtual void connect() override final;

private:
	explicit SDLSocket(TCPsocket socket_, IPaddress *ip_, const char *remoteHost_, uint16_t port_);

	TCPsocket internalSocket;
	IPaddress internalIP;
};

class SDLAcceptorSocket final : public AcceptorSocket {
public:
	explicit SDLAcceptorSocket(uint16_t port, bool autoListen = false);
	virtual ~SDLAcceptorSocket();

	virtual std::unique_ptr<Socket> acceptSocket(float maxWaitInSeconds = -1.0f) override final;
	virtual std::unique_ptr<Socket> acceptSocketOnce() override final;

	const TCPsocket &getSDLAcceptor() const {
		return internalAcceptor;
	}

protected:
	virtual void listen() override final;

private:
	TCPsocket internalAcceptor;
	IPaddress internalIP;

	float waitTime = 0.0f;
};

}

#endif /* INCLUDE_APG_NET_SDLSOCKET_HPP_ */
