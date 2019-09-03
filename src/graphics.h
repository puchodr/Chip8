#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include <SDL2/SDL.h>

struct SDL_Texture;
struct SDL_Rect;

struct Graphics
{
    Graphics();
    ~Graphics();

    SDL_Texture* loadImage(const std::string& file_name, bool black_is_transparent=false);

    void renderTexture(SDL_Texture *texture,
         const SDL_Rect destination,
         const SDL_Rect *clip=nullptr) const;
    void renderTexture(SDL_Texture *texture,
         int x, int y,
         const SDL_Rect *clip=nullptr) const;

    void clear() const;
    void flip() const;
    void toggleFullscreen();

private:
    typedef std::map<std::string, SDL_Texture*> SpriteMap;
    SpriteMap sprite_sheets_;
    SDL_Window *sdl_window_;
    SDL_Renderer *sdl_renderer_;
    bool fullscreen_;
};

#endif // GRAPHICS_H_
