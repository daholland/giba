#include <spock.h>
#include <gibalib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "Tracy.hpp"
#include "TracyVulkan.hpp"

#include "config.h"
#include "ui.h"

using namespace spock;
using namespace gibalib;



void run(Spock& spock, Gibalib& gb, UiState& uiState, AppConfig& appConfig) {
    SDL_Event e;
    bool bQuit = false;

    Input gbInput;

    while (!bQuit) {
        ZoneScopedN("Main Loop");
        {
            ZoneScopedNC("Event Loop", tracy::Color::White);

            while (SDL_PollEvent(&e) != 0) {
                process_imgui_event(&e, uiState);

                if (e.type == SDL_QUIT
                    //|| (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)
                        ) {
                    bQuit = true;
                }

                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == appConfig.Controls.Quit) {
                        SDL_Event ev;
                        ev.type = SDL_QUIT;

                        SDL_PushEvent(&ev);
                    }

                    if (e.key.keysym.sym == appConfig.Controls.ToggleGui) {
                        uiState.p_open = !uiState.p_open;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Button_A) {
                        gbInput.A = true;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Button_B) {
                        gbInput.B = true;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Button_Start) {
                        gbInput.Start = true;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Button_Select) {
                        gbInput.Select = true;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Up) {
                        gbInput.Up = true;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Down) {
                        gbInput.Down = true;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Left) {
                        gbInput.Left = true;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Right) {
                        gbInput.Right = true;
                    }
                }

                if (e.type == SDL_KEYUP) {
                    if (e.key.keysym.sym == appConfig.Controls.Button_A) {
                        gbInput.A = false;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Button_B) {
                        gbInput.B = false;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Button_Start) {
                        gbInput.Start = false;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Button_Select) {
                        gbInput.Select = false;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Up) {
                        gbInput.Up = false;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Down) {
                        gbInput.Down = false;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Left) {
                        gbInput.Left = false;
                    }

                    if (e.key.keysym.sym == appConfig.Controls.Right) {
                        gbInput.Right = false;
                    }
                }
            }

            gb.set_input(gbInput);
        }

        run_imgui(spock, gb, uiState);

        spock.draw();
    }
}

int main(int argc, char* argv[]) {
    auto spock = std::make_unique<Spock>();
    auto gb = std::make_unique<Gibalib>();

    auto uiState = std::make_unique<UiState>();
    auto config = std::make_unique<AppConfig>();

    std::filesystem::path filepath = "../assets/roms/Tetris (World) (Rev 1).zip";
    gb->load_cart_from_file(filepath);
    auto quuz = "asdf";
    spock->init();

    gb->foobar();

    run(*spock, *gb, *uiState, *config);

    spock->cleanup();

    gb->cleanup();

    return 0;
}
