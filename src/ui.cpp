#include <spock.h>
#include <gibalib.h>

#include <SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_vulkan.h>
#include "imgui_memory_editor.h"

#include "Tracy.hpp"
#include "TracyVulkan.hpp"

#include "ui.h"
#include <fmt/core.h>

void process_imgui_event(SDL_Event* ev, UiState& ui) {
    ImGui_ImplSDL2_ProcessEvent(ev);
}

void run_imgui(spock::Spock& spock, gibalib::Gibalib& gb, UiState& ui)
{
    ZoneScopedNC("Imgui Logic", tracy::Color::Grey);

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(spock._window);
    ImGui::NewFrame();

    const auto& gbInput = gb.get_input();
    const auto& gbCart = gb.get_cart();

        {

            if (ui.p_open) {
                ImGui::SetNextWindowSize(ImVec2(575, 600), ImGuiCond_FirstUseEver);
                if (!ImGui::Begin("Giba wats", &ui.p_open, ui.window_flags)) {
                    // Early out if the window is collapsed, as an optimization.
                    ImGui::End();
                    return;
                }

                ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

                static MemoryEditor mem_edit;
                mem_edit.ReadOnly = true;
                static char data[0x10000];
                size_t data_size = 0x10000;

                auto inputCopy = gbInput;
                auto cartCopy = gbCart;

                uint8_t inputbyte = inputCopy.as_byte();

                if (ImGui::BeginMenuBar()) {
                    if (ImGui::BeginMenu("Giba Menu")) {
                        ImGui::MenuItem("File", nullptr, &ui.show_main_menu_bar);
                        ImGui::EndMenu();
                    }

                    ImGui::EndMenuBar();
                }
                ImGui::Text("here it is, giba!");

                if (ImGui::CollapsingHeader("CPU")){
                    ImGui::Text("Info about CPU");
                }

                if (ImGui::CollapsingHeader("Memory")){

                    ImGui::Text("Info about Memory");
                    //mem_edit.DrawContents(data, data_size);
                }

                if (ImGui::CollapsingHeader("Cart")){
                    ImGui::Text("Info about Cart");
                    mem_edit.DrawContents(cartCopy.rom.data(), cartCopy.rom.size());
                }

                if (ImGui::CollapsingHeader("PPU")){
                    ImGui::Text("Info about PPU");
                }

                if (ImGui::CollapsingHeader("Input")){
                    ImGui::Text("Info about Input");
                    ImGui::Checkbox("(A)", &inputCopy.A);
                    ImGui::Checkbox("(B)", &inputCopy.B);
                    ImGui::Checkbox("(Start)", &inputCopy.Start);
                    ImGui::Checkbox("(Select)", &inputCopy.Select);
                    ImGui::Checkbox("(Up)", &inputCopy.Up);
                    ImGui::Checkbox("(Down)", &inputCopy.Down);
                    ImGui::Checkbox("(Left)", &inputCopy.Left);
                    ImGui::Checkbox("(Right)", &inputCopy.Right);

                    std::string inputbyte_fmt = fmt::format("binary: {0:b}", inputbyte);

                    ImGui::Text(inputbyte_fmt.c_str());

                    mem_edit.DrawContents(&inputbyte, sizeof(uint8_t));



                }

                if (ImGui::CollapsingHeader("Audio")){
                    ImGui::Text("Info about Audio");
                }

                ImGui::PopItemWidth();
                ImGui::End();
            }
        }

    ImGui::ShowDemoWindow();
}
