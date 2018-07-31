#ifndef INCLUDE_TEST_APGSDLRENDERTEST_HPP_
#define INCLUDE_TEST_APGSDLRENDERTEST_HPP_

#include <memory>

#include "APG/SXXDL.hpp"
#include "APG/core/Game.hpp"
#include "APG/core/SDLGame.hpp"
#include "APG/core/APGCommon.hpp"
#include "APG/tiled/SDLTmxRenderer.hpp"

class APGSDLRenderTest final : public APG::SDLGame {
public:
	APGSDLRenderTest(APG::APGContext &context) :
			SDLGame(context) {
	}
	virtual ~APGSDLRenderTest() = default;

	bool init() override;
	void render(float deltaTime) override;

private:
	SXXDL::renderer_ptr renderer = SXXDL::make_renderer_ptr(nullptr);

	std::unique_ptr<APG::SDLTmxRenderer> rendererOne;
	std::unique_ptr<APG::SDLTmxRenderer> rendererTwo;

	APG::SDLTmxRenderer *currentRenderer = nullptr;
};

#endif /* APGSDLRENDERTEST_HPP_ */
