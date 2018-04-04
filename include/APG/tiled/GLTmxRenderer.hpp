#ifndef GLTMXRENDERER_HPP_
#define GLTMXRENDERER_HPP_

#ifndef APG_NO_SDL
#ifndef APG_NO_GL

#include "APG/graphics/SpriteBatch.hpp"
#include "APG/tiled/TmxRenderer.hpp"

namespace Tmx {
class Layer;
}

namespace APG {

class GLTmxRenderer final : public TmxRenderer<GLTmxRenderer> {
public:
	explicit GLTmxRenderer(std::unique_ptr<Tmx::Map> &&map, SpriteBatch * const batch);
	explicit GLTmxRenderer(Tmx::Map * const map, SpriteBatch * const batch);
	explicit GLTmxRenderer(const std::string &fileName, SpriteBatch * const batch);
	virtual ~GLTmxRenderer() = default;

	virtual void renderAll(float deltaTime);

protected:
	friend class TmxRenderer<GLTmxRenderer>;
	void renderLayerImpl(const Tmx::TileLayer * layer);
	void renderObjectGroupImpl(const std::vector<TiledObject> &objects);

private:
	SpriteBatch *batch;
};

}

#endif
#endif

#endif /* GLTMXRENDERER_HPP_ */
