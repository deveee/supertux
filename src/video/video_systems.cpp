//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <config.h>

#include "supertux/gameconfig.hpp"
#include "video/gl/gl_lightmap.hpp"
#include "video/gl/gl_renderer.hpp"
#include "video/gl/gl_surface_data.hpp"
#include "video/gl/gl_texture.hpp"
#include "video/lightmap.hpp"
#include "video/renderer.hpp"
#include "video/sdl/sdl_lightmap.hpp"
#include "video/sdl/sdl_renderer.hpp"
#include "video/sdl/sdl_surface_data.hpp"
#include "video/sdl/sdl_texture.hpp"
#include "video/texture.hpp"
#include "video/video_systems.hpp"

Renderer *new_renderer()
{
  switch(config->video)
  {
    case AUTO_VIDEO:
#ifdef HAVE_OPENGL
      log_info << "new GL renderer\n";
      return new GLRenderer();
#else
      log_warning << "new SDL renderer\n";
      return new SDLRenderer();
#endif
#ifdef HAVE_OPENGL
    case OPENGL:
      log_info << "new GL renderer\n";
      return new GLRenderer();
#endif
    case PURE_SDL:
      log_warning << "new SDL renderer\n";
      return new SDLRenderer();
    default:
      assert(0 && "invalid video system in config");
#ifdef HAVE_OPENGL
      log_info << "new GL renderer\n";
      return new GLRenderer();
#else
      log_warning << "new SDL renderer\n";
      return new SDLRenderer();
#endif
  }
}

Lightmap *new_lightmap()
{
  switch(config->video)
  {
    case AUTO_VIDEO:
#ifdef HAVE_OPENGL
      return new GLLightmap();
#else
      return new SDLLightmap();
#endif
#ifdef HAVE_OPENGL
    case OPENGL:
      return new GLLightmap();
#endif
    case PURE_SDL:
      return new SDLLightmap();
    default:
      assert(0 && "invalid video system in config");
#ifdef HAVE_OPENGL
      return new GLLightmap();
#else
      return new SDLLightmap();
#endif
  }
}

Texture *new_texture(SDL_Surface *image)
{
  switch(config->video)
  {
    case AUTO_VIDEO:
#ifdef HAVE_OPENGL
      return new GLTexture(image);
#else
      return new SDLTexture(image);
#endif
#ifdef HAVE_OPENGL
    case OPENGL:
      return new GLTexture(image);
#endif
    case PURE_SDL:
      return new SDLTexture(image);
    default:
      assert(0 && "invalid video system in config");
#ifdef HAVE_OPENGL
      return new GLTexture(image);
#else
      return new SDLTexture(image);
#endif
  }
}

void *new_surface_data(const Surface &surface)
{
  switch(config->video)
  {
    case AUTO_VIDEO:
#ifdef HAVE_OPENGL
      return new GLSurfaceData(surface);
#else
      return new SDLSurfaceData(surface);
#endif
#ifdef HAVE_OPENGL
    case OPENGL:
      return new GLSurfaceData(surface);
#endif
    case PURE_SDL:
      return new SDLSurfaceData(surface);
    default:
      assert(0 && "invalid video system in config");
#ifdef HAVE_OPENGL
      return new GLSurfaceData(surface);
#else
      return new SDLSurfaceData(surface);
#endif
  }
}

void free_surface_data(void *surface_data)
{
  delete reinterpret_cast<char *>(surface_data);
}

VideoSystem get_video_system(const std::string &video)
{
  if(video == "auto")
  {
    return AUTO_VIDEO;
  }
#ifdef HAVE_OPENGL
  else if(video == "opengl")
  {
    return OPENGL;
  }
#endif
  else if(video == "sdl")
  {
    return PURE_SDL;
  }
  else
  {
    return AUTO_VIDEO;
  }
}

std::string get_video_string(VideoSystem video)
{
  switch(video)
  {
    case AUTO_VIDEO:
      return "auto";
    case OPENGL:
      return "opengl";
    case PURE_SDL:
      return "sdl";
    default:
      assert(0 && "invalid video system in config");
      return "auto";
  }
}

/* EOF */