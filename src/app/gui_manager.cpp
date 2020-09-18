#include "gui_manager.h"
#include "display.h"
#include "input.h"
#include "types.h"
#include <imgui/imgui.h>
#include <map>

namespace app {
static std::map<ImGuiMouseCursor, Display::CursorShape> cursorShapeMap;

GuiManager::GuiManager(Input &input)
    : io((IMGUI_CHECKVERSION(), ImGui::CreateContext(), ImGui::StyleColorsDark(), ImGui::GetIO())) {
  /* Init IMGUI */
  io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  /* map cursor shapes  */
  cursorShapeMap[ImGuiMouseCursor_Arrow] = Display::CursorShape::ARROW;
  cursorShapeMap[ImGuiMouseCursor_TextInput] = Display::CursorShape::IBEAM;
  cursorShapeMap[ImGuiMouseCursor_Hand] = Display::CursorShape::HAND;
  cursorShapeMap[ImGuiMouseCursor_ResizeNS] = Display::CursorShape::VRESIZE;
  cursorShapeMap[ImGuiMouseCursor_ResizeEW] = Display::CursorShape::HRESIZE;

  mapInput(input);
}

void GuiManager::mapInput(Input &input) {
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
  input.addKeyCallback(Input::Key::ANY, [&io = io](const Input::KeyEvent keyEvent) {
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

  // charCallback
  input.addCharCallback([&io = io](Input::CharEvent e) { io.AddInputCharacter(e.characer); });

  // scrollCallback
  input.addScrollOffsetCallback([&io = io](Input::ScrollOffset offset) {
    io.MouseWheelH += offset.x;
    io.MouseWheel += offset.y;
  });

  // mousebuttonCallback
  input.addMouseButton(Input::MouseButton::ANY, [&io = io](const Input::MouseButtonEvent event) {
    u8 btnCode = toUnderlying<Input::MouseButton>(event.button);
    if (btnCode < ImGuiMouseButton_COUNT && event.action == Input::Action::PRESS)
      io.MouseDown[btnCode] = true;
  });
  input.setCursorModeCallback([&io = io](const Input::CursorMode cursorMode) {
    if (cursorMode == Input::CursorMode::NORMAL)
      io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    else
      io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
  });
}

void GuiManager::showDockSpace() {
  static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
  // because it would be confusing to have two docking targets within each others.
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->GetWorkPos());
  ImGui::SetNextWindowSize(viewport->GetWorkSize());
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  ImGui::Begin("3DRenderer", nullptr, windowFlags);
  ImGui::PopStyleVar(2);

  // DockSpace
  ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
  ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
  ImGui::End();
}

void GuiManager::newFrame(float dt, Input &input, Display &display) {
  ImGuiIO &io = ImGui::GetIO();
  // TODO: move crusor status get/set to display

  // cursor pos
  if (display.isFocused()) {
    if (io.WantSetMousePos) {
      const ImVec2 mousePos = io.MousePos;
      input.setCursorPos({mousePos.x, mousePos.y});
      io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    } else {
      const Input::CursorPos pos = input.getLastCursorPos();
      io.MousePos = ImVec2(pos.x, pos.y);
    }
  }

  // buttton
  for (int i = 0; i < ImGuiMouseButton_COUNT; i++) {
    io.MouseDown[i] = input.getButton(static_cast<Input::MouseButton>(i));
  }

  // update cursor
  ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
  if (imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    input.setCursorMode(Input::CursorMode::HIDDEN);
  else {
    auto shapeIt = cursorShapeMap.find(imguiCursor);
    /* If cursor doesn't exists default to arrow. */
    Display::CursorShape cursorShape =
        (shapeIt == cursorShapeMap.end()) ? Display::CursorShape::ARROW : shapeIt->second;
    display.setCursorShape(cursorShape);
    //    input.setCursorMode(Input::CursorMode::NORMAL);
  }

  // TODO: add this to resize callback
  // set framebuffer size - set every frame for resize support
  glm::ivec2 displaySize = display.getDisplaySize();
  glm::ivec2 fboSize = display.getFboSize();

  io.DisplaySize = ImVec2((float)displaySize.x, (float)displaySize.y);
  // ratio of framebufferSize & displaySize
  // coz in retian display fboSize != displaySize (fboSize is about 2xdisplaySize)
  if (displaySize.x > 0 && displaySize.y > 0)
    io.DisplayFramebufferScale =
        ImVec2((float)fboSize.x / displaySize.x, (float)fboSize.y / displaySize.y);

  // set delta
  io.DeltaTime = dt;
  ImGui::NewFrame();
  showDockSpace();
  ImGui::ShowDemoWindow();
}
}; // namespace app

static_assert(ImGuiMouseButton_COUNT == 5 && "Imgui MouseButton_COUNT changed.");
