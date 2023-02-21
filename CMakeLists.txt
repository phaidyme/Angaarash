cmake_minimum_required(VERSION 3.7...3.25)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(Angaarash LANGUAGES CXX)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS false)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_SKIP_BUILD_RPATH false) # use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_BUILD_WITH_INSTALL_RPATH false) # when building, don't use the install RPATH already
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib") # but do use it later on when installing
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true) # add the automatically determined parts of the RPATH which point to directories outside the build tree to the install RPATH

set(GLFW_BUILD_DOCS false CACHE BOOL false FORCE)
set(GLFW_BUILD_TESTS false CACHE BOOL false FORCE)
set(GLFW_BUILD_EXAMPLES false CACHE BOOL false FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/ext/glfw)

add_library(ImGui SHARED
	${PROJECT_SOURCE_DIR}/ext/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/ext/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/ext/imgui/imgui_tables.cpp
	${PROJECT_SOURCE_DIR}/ext/imgui/imgui_widgets.cpp
	${PROJECT_SOURCE_DIR}/ext/imgui/backends/imgui_impl_glfw.cpp
	${PROJECT_SOURCE_DIR}/ext/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(ImGui PRIVATE "${PROJECT_SOURCE_DIR}/ext/imgui")
target_include_directories(ImGui PRIVATE "${PROJECT_SOURCE_DIR}/ext/imgui/backends")

target_link_libraries(ImGui glfw)
target_link_libraries(ImGui OpenGL::GL)

add_subdirectory(src)

install(TARGETS Angaarash DESTINATION bin)
install(TARGETS ImGui DESTINATION lib)
install(FILES "${PROJECT_SOURCE_DIR}/LICENSE.txt" DESTINATION .)
install(FILES "${PROJECT_SOURCE_DIR}/README.md" DESTINATION .)

include(InstallRequiredSystemLibraries)
include(CPack)
