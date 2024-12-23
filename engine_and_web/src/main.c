#include <SDL2/SDL_image.h>
#include "engine/game.h"
#include "engine/common.h"
#include "engine/sprite.h"
#include "engine/interface.h"
#include "engine/timers.h"
#include "engine/audio.h"

#define WIDTH 640
#define HEIGHT 640

static struct
{
    ng_game_t game;
    ng_interval_t game_tick;

    // A collection of assets used by entities
    // Ideally, they should have been automatically loaded
    // by iterating over the res/ folder and filling in a hastable
    SDL_Texture *player_texture, *explosion_texture;
    Mix_Chunk *gem_sfx;
    TTF_Font *main_font;

    ng_sprite_t player;
    ng_label_t welcome_text;
    ng_animated_sprite_t explosion;
} ctx;

static void create_actors(void)
{
    ng_game_create(&ctx.game, "Hello World!", WIDTH, HEIGHT);

    ctx.main_font = TTF_OpenFont("res/free_mono.ttf", 16);
    ctx.player_texture = IMG_LoadTexture(ctx.game.renderer, "res/player.png");
    ctx.explosion_texture = IMG_LoadTexture(ctx.game.renderer, "res/explosion.png");
    ctx.gem_sfx = ng_audio_load("res/gem.wav");

    ng_interval_create(&ctx.game_tick, 50);
    ng_sprite_create(&ctx.player, ctx.player_texture);
    ng_sprite_set_scale(&ctx.player, 2.0f);
    ctx.player.transform.y = HEIGHT - ctx.player.transform.h - 10;

    ng_animated_create(&ctx.explosion, ctx.explosion_texture, 9);
    ng_sprite_set_scale(&ctx.explosion.sprite, 2.0f);
    ctx.explosion.sprite.transform.x = 200.0f;

    ng_label_create(&ctx.welcome_text, ctx.main_font, 300);
    ng_label_set_content(&ctx.welcome_text, ctx.game.renderer,
            "I don't really know what to write in here. "
            "You'll notice that you can't type Greek. To do that, "
            "just replace the internals with TTF_RenderUTF8_Solid");
}

// A place to handle queued events.
static void handle_event(SDL_Event *event)
{
    switch (event->type)
    {
    case SDL_KEYDOWN:
        // Press space for a sound effect!
        if (event->key.keysym.sym == SDLK_SPACE)
            ng_audio_play(ctx.gem_sfx);

        break;

    case SDL_MOUSEMOTION:
        // Move label on mouse position
        // By the way, that's how you can implement a custom cursor
        ctx.welcome_text.sprite.transform.x = event->motion.x;
        ctx.welcome_text.sprite.transform.y = event->motion.y;
        break;
    }
}

static void update_and_render_scene(float delta)
{
    // Handling "continuous" events, which are now repeatable
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) ctx.player.transform.x -= 640* delta;
    if (keys[SDL_SCANCODE_RIGHT]) ctx.player.transform.x += 640* delta;

    // Update the explosion's frame once every 100ms
    if (ng_interval_is_ready(&ctx.game_tick))
    {
        ng_animated_set_frame(&ctx.explosion, (ctx.explosion.frame + 1)
                % ctx.explosion.total_frames);
    }

    ng_sprite_render(&ctx.player, ctx.game.renderer);
    ng_sprite_render(&ctx.explosion.sprite, ctx.game.renderer);
    ng_sprite_render(&ctx.welcome_text.sprite, ctx.game.renderer);
}

int main()
{
    create_actors();
    ng_game_start_loop(&ctx.game,
            handle_event, update_and_render_scene);
}
