#include "graphics.h"

#include <stdexcept>
#include <SDL2/SDL_image.h>

#include "chip8.h"

namespace
{
    const int kBitsPerPixel = 32;
}

Graphics::Graphics()
{
    sdl_window = SDL_CreateWindow(
            "Chip-8",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            Chip8::SCREEN_WIDTH,
            Chip8::SCREEN_HEIGHT,
            SDL_WINDOW_OPENGL);

    sdl_renderer = SDL_CreateRenderer(
            sdl_window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(sdl_renderer,
            Chip8::SCREEN_WIDTH,
            Chip8::SCREEN_HEIGHT);

    if (sdl_window == nullptr)
        throw std::runtime_error("SDL_CreateWindow");
    if (sdl_renderer == nullptr)
        throw std::runtime_error("SDL_CreateRenderer");

    //SDL_ShowCursor(SDL_DISABLE);

    fullscreen = false;
}

Graphics::~Graphics()
{
    for (SpriteMap::iterator iter = sprite_sheets.begin(); iter != sprite_sheets.end(); ++iter)
    {
        SDL_DestroyTexture(iter->second);
    }

    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
}

SDL_Texture* Graphics::load_image(const std::string& file_name, bool black_is_transparent)
{
    const std::string file_path = "../content/" + file_name + ".bmp";

    // Sprite cache, so we're not loading the same images over and over again.
    if (sprite_sheets.count(file_path) == 0)
    {
        SDL_Texture *texture;
        if (black_is_transparent)
        {
            SDL_Surface *surface = SDL_LoadBMP(file_path.c_str());
            if (surface == nullptr)
            {
                std::string error = "Cannot load texture '" + file_path + "'!";
                throw std::runtime_error(error);
            }
            const Uint32 black_color = SDL_MapRGB(surface->format, 0, 0, 0);
            SDL_SetColorKey(surface, SDL_TRUE, black_color);
            texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
            SDL_FreeSurface(surface);
        }
        else
        {
            texture = IMG_LoadTexture(sdl_renderer, file_path.c_str());
        }
        if (texture == nullptr)
        {
            throw std::runtime_error("Cannot load texture!");
        }
        sprite_sheets[file_path] = texture;
    }
    return sprite_sheets[file_path];
}

void Graphics::render_texture(SDL_Texture *texture,
        const SDL_Rect destination,
        const SDL_Rect *clip) const
{
    SDL_RenderCopy(sdl_renderer, texture, clip, &destination);
}

void Graphics::render_texture(SDL_Texture *texture,
        int x, int y,
        const SDL_Rect *clip) const
{
    SDL_Rect destination;
    destination.x = x;
    destination.y = y;

    if (clip != nullptr)
    {
        destination.w = clip->w;
        destination.h = clip->h;
    }
    else
    {
        SDL_QueryTexture(texture, nullptr, nullptr, &destination.w, &destination.h);
    }

    render_texture(texture, destination, clip);
}

void Graphics::clear() const
{
    SDL_RenderClear(sdl_renderer);
}

void Graphics::flip() const
{
    SDL_RenderPresent(sdl_renderer);
}

void Graphics::toggle_fullscreen()
{
    Uint32 flags = (SDL_GetWindowFlags(sdl_window) ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
    int error = SDL_SetWindowFullscreen(sdl_window, flags);
    if (error < 0)
    {
        throw std::runtime_error("Cannot make window fullscreen!");
    }
}
