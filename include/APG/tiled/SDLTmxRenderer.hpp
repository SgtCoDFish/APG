#ifndef INCLUDE_APG_TILED_TMXSDLRENDERER_HPP_
#define INCLUDE_APG_TILED_TMXSDLRENDERER_HPP_

#ifndef APG_NO_SDL

#include <vector>

#include "APG/SXXDL.hpp"
#include "APG/core/APGCommon.hpp"
#include "APG/tiled/TmxRenderer.hpp"

#include <glm/vec2.hpp>

namespace Tmx {
class Tileset;

class Layer;
}

namespace APG {

/**
 * Contains methods for rendering a loaded TMX file using SDL2.
 *
 * Requires that SDL2 + SDL2_image have already been initialised. Doesn't yet support animated tiles.
 */
class SDLTmxRenderer final : public TmxRenderer<SDLTmxRenderer> {
public:
	explicit SDLTmxRenderer(std::unique_ptr<Tmx::Map> &&map, const SXXDL::renderer_ptr &renderer);

	explicit SDLTmxRenderer(Tmx::Map *map, const SXXDL::renderer_ptr &renderer);

	explicit SDLTmxRenderer(const std::string &fileName, const SXXDL::renderer_ptr &renderer);

	virtual ~SDLTmxRenderer() = default;

	// disallow copying because we own resources.
	SDLTmxRenderer(SDLTmxRenderer &other) = delete;

	SDLTmxRenderer(const SDLTmxRenderer &other) = delete;

protected:
	friend class TmxRenderer<SDLTmxRenderer>;

	void renderLayerImpl(const Tmx::TileLayer *layer);

	void renderObjectGroupImpl(const std::vector<TiledObject> &objects);

private:
	const SXXDL::renderer_ptr &renderer;

	std::vector<SXXDL::sdl_texture_ptr> sdlTextures;

	void setupTilesets();
};

}

#endif

#endif /* TMXSDLRENDERER_HPP_ */
