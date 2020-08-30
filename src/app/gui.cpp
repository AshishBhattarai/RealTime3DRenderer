#include "gui.h"
#include "input.h"
#include "types.h"
#include <imgui/imgui.h>

namespace app {

Gui::Gui(Input &input) {
  /* Init IMGUI */
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGuiIO &io = ImGui::GetIO();
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

  mapInput(input);
}

void Gui::mapInput(Input &input) {
  ImGuiIO &io = ImGui::GetIO();
  /* Keyboard mapping */
  io.KeyMap[ImGuiKey_Tab] = toUnderlying<Input::Key>(Input::Key::TAB);
  io.KeyMap[ImGuiKey_LeftArrow] = toUnderlying<Input::Key>(Input::Key::LEFT);
  io.KeyMap[ImGuiKey_RightArrow] = toUnderlying<Input::Key>(Input::Key::RIGHT);
  io.KeyMap[ImGuiKey_UpArrow] = toUnderlying<Input::Key>(Input::Key::UP);
  io.KeyMap[ImGuiKey_DownArrow] = toUnderlying<Input::Key>(Input::Key::DOWN);
  io.KeyMap[ImGuiKey_PageUp] = toUnderlying<Input::Key>(Input::Key::PAGE_UP);
  io.KeyMap[ImGuiKey_PageDown] = toUnderlying<Input::Key>(Input::Key::PAGE_DOWN);
  io.KeyMap[ImGuiKey_Home] = toUnderlying<Input::Key>(Input::Key::HOME);
  io.KeyMap[ImGuiKey_End] = toUnderlying<Input::Key>(Input::Key::END);
  io.KeyMap[ImGuiKey_Insert] = toUnderlying<Input::Key>(Input::Key::INSERT);
  io.KeyMap[ImGuiKey_Delete] = toUnderlying<Input::Key>(Input::Key::DELETE);
  io.KeyMap[ImGuiKey_Backspace] = toUnderlying<Input::Key>(Input::Key::BACKSPACE);
  io.KeyMap[ImGuiKey_Space] = toUnderlying<Input::Key>(Input::Key::SPACE);
  io.KeyMap[ImGuiKey_Enter] = toUnderlying<Input::Key>(Input::Key::ENTER);
  io.KeyMap[ImGuiKey_Escape] = toUnderlying<Input::Key>(Input::Key::ESCAPE);
  io.KeyMap[ImGuiKey_KeyPadEnter] = toUnderlying<Input::Key>(Input::Key::KP_ENTER);
  io.KeyMap[ImGuiKey_A] = toUnderlying<Input::Key>(Input::Key::A);
  io.KeyMap[ImGuiKey_C] = toUnderlying<Input::Key>(Input::Key::C);
  io.KeyMap[ImGuiKey_V] = toUnderlying<Input::Key>(Input::Key::V);
  io.KeyMap[ImGuiKey_X] = toUnderlying<Input::Key>(Input::Key::X);
  io.KeyMap[ImGuiKey_Y] = toUnderlying<Input::Key>(Input::Key::Y);
  io.KeyMap[ImGuiKey_Z] = toUnderlying<Input::Key>(Input::Key::Z);

  /* Callbacks */
  input.addKeyCallback(Input::Key::ANY, [&io](const Input::KeyEvent &keyEvent) {
    io.KeysDown[toUnderlying<Input::Key>(keyEvent.key)] =
        (keyEvent.action == Input::Action::PRESS) || (keyEvent.action == Input::Action::REPEAT);
    io.KeysDown[toUnderlying<Input::Key>(keyEvent.key)] = keyEvent.action == Input::Action::RELEASE;

    // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[toUnderlying<Input::Key>(Input::Key::LEFT_CONTROL)] ||
                 io.KeysDown[toUnderlying<Input::Key>(Input::Key::RIGHT_CONTROL)];
    io.KeyShift = io.KeysDown[toUnderlying<Input::Key>(Input::Key::LEFT_SHIFT)] ||
                  io.KeysDown[toUnderlying<Input::Key>(Input::Key::RIGHT_SHIFT)];
    io.KeyAlt = io.KeysDown[toUnderlying<Input::Key>(Input::Key::LEFT_ALT)] ||
                io.KeysDown[toUnderlying<Input::Key>(Input::Key::RIGHT_ALT)];
    // Super should be disabled on windows (see imgui examples & docs) TODO: WIN32 support
    io.KeySuper = io.KeysDown[toUnderlying<Input::Key>(Input::Key::LEFT_SUPER)] ||
                  io.KeysDown[toUnderlying<Input::Key>(Input::Key::RIGHT_SUPER)];
  });
  /**
   * TODO: charCallback
   * TODO: scrollCallback
   * TODO: mousebuttonCallback
   */
}

}; // name space app
