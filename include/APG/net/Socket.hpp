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

#ifndef INCLUDE_APG_NET_SOCKET_HPP_
#define INCLUDE_APG_NET_SOCKET_HPP_

#include <cstdint>

#include <memory>
#include <array>

#include "ByteBuffer.hpp"

namespace APG {
/**
 * Simple base class for a socket.
 *
 * You'll probably want a derived class of this.
 */
class Socket : public ByteBuffer {
public:
	explicit Socket(const char * remoteHost, uint16_t port, uint32_t bufferSize = BB_DEFAULT_SIZE);
	virtual ~Socket() = default;

	const uint16_t port;
	const char * const remoteHost;

	/**
	 * Send data currently in the sendBuffer.
	 */
	virtual int send() = 0;

	/**
	 * Read data into readBuffer.
	 */
	virtual int recv(uint32_t length = 1024u) = 0;

	bool hasError() const {
		return error_;
	}

protected:
	void setError();

private:
	bool error_;
};

/**
 * Also known as a server socket, accepting incoming connections and generating sockets for them.
 */
class AcceptorSocket : public ByteBuffer {
public:
	explicit AcceptorSocket(uint16_t port, uint32_t bufferSize = BB_DEFAULT_SIZE);
	virtual ~AcceptorSocket() = default;

	const uint16_t port;

	virtual std::unique_ptr<Socket> acceptSocket(float maxWaitInSeconds = -1.0f) = 0;

	bool hasError() const {
		return error_;
	}

protected:
	void setError();

private:
	bool error_;
};

}

#endif /* INCLUDE_APG_NET_SOCKET_HPP_ */
