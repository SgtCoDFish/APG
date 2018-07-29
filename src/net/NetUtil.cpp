#include <cstdint>

#include "APG/net/NetUtil.hpp"

#include "spdlog/spdlog.h"

namespace APG {

bool NetUtil::validatePort(uint16_t port, bool shouldLog) {
	auto logger = spdlog::get("APG");

	if (port == 0u) {
		if (shouldLog) {
			logger->error("Ports cannot open on 0.");
		}

		return false;
	}

	if (port <= 1024u) {
		if(shouldLog) {
			logger->warn("Port {} is a reserved port and probably shouldn't be used.", port);
		}

		return false;
	}

	return true;
}

}
