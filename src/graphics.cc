#include "graphics.h"

#include <stdexcept>
#include <SDL2/SDL_image.h>

#include "chip8.h"

Graphics::Graphics(std::string title, int window_width, int window_height, int texture_width, int texture_height)
{
    window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            window_width,
            window_height,
            SDL_WINDOW_OPENGL);

    renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING,
            texture_width,
            texture_height);

    // @Todo: Figure out if we need these. If so, we may need to also use
    // SDL_RenderSetIntegerScale(renderer, true);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer,
            Chip8::SCREEN_WIDTH,
            Chip8::SCREEN_HEIGHT);

    if (window == nullptr)
        throw std::runtime_error("SDL_CreateWindow");
    if (renderer == nullptr)
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

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void Graphics::update_window(void const* buffer, int pitch) const
{
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    clear();
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    flip();
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
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
        else
        {
            texture = IMG_LoadTexture(renderer, file_path.c_str());
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
    SDL_RenderCopy(renderer, texture, clip, &destination);
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
    SDL_RenderClear(renderer);
}

void Graphics::flip() const
{
    SDL_RenderPresent(renderer);
}

void Graphics::toggle_fullscreen()
{
    Uint32 flags = (SDL_GetWindowFlags(window) ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
    int error = SDL_SetWindowFullscreen(window, flags);
    if (error < 0)
    {
        throw std::runtime_error("Cannot make window fullscreen!");
    }
}
