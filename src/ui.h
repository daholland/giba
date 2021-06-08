#ifndef GIBA_UI_H
#define GIBA_UI_H

#include "SDL_events.h"
#include "gibalib.h"
#include "spock.h"
#include <imgui.h>

struct UiState {
    ImGuiWindowFlags window_flags = 0;
    bool p_open = true;
    bool show_main_menu_bar = false;
};

void process_imgui_event(SDL_Event* ev, UiState& ui);
void run_imgui(spock::Spock& spock, gibalib::Gibalib& gb, UiState& ui);

#endif //GIBA_UI_H
