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
    Graphics(std::string title, int window_width, int window_height, int texture_width, int texture_height);
    ~Graphics();

    SDL_Texture* load_image(const std::string& file_name, bool black_is_transparent=false);

    void update_window(void const* buffer, int pitch) const;

    void render_texture(SDL_Texture *texture,
         const SDL_Rect destination,
         const SDL_Rect *clip=nullptr) const;
    void render_texture(SDL_Texture *texture,
         int x, int y,
         const SDL_Rect *clip=nullptr) const;

    void clear() const;
    void flip() const;
    void toggle_fullscreen();

private:
    typedef std::map<std::string, SDL_Texture*> SpriteMap;
    SpriteMap sprite_sheets;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    bool fullscreen;
};

#endif // GRAPHICS_H_
