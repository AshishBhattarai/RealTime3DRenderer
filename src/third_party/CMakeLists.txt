#glad
add_subdirectory(glad)

#imgui
add_library(${IMGUI_LIBRARIES}
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
)
