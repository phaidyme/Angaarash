cmake_minimum_required(VERSION 3.7...3.25)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(Angaarash LANGUAGES CXX)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
add_subdirectory(${PROJECT_SOURCE_DIR}/ext/glfw)

add_library(ImGui STATIC
	${PROJECT_SOURCE_DIR}/ext/imgui/imgui.cpp ${PROJECT_SOURCE_DIR}/ext/imgui/imgui_draw.cpp ${PROJECT_SOURCE_DIR}/ext/imgui/imgui_tables.cpp ${PROJECT_SOURCE_DIR}/ext/imgui/imgui_widgets.cpp
	${PROJECT_SOURCE_DIR}/ext/imgui/backends/imgui_impl_glfw.cpp ${PROJECT_SOURCE_DIR}/ext/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(ImGui PRIVATE ${PROJECT_SOURCE_DIR}/ext/imgui)
target_include_directories(ImGui PRIVATE ${PROJECT_SOURCE_DIR}/ext/imgui/backends)

target_link_libraries(ImGui glfw)
target_link_libraries(ImGui OpenGL::GL)

add_subdirectory(src)

install(TARGETS Angaarash DESTINATION Release/bin/)
install(FILES ${PROJECT_SOURCE_DIR}/LICENSE.txt DESTINATION Release/)

include(InstallRequiredSystemLibraries)
include(CPack)
