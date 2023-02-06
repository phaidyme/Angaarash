cmake_minimum_required(VERSION 3.7...3.25)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(Engaar LANGUAGES CXX)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(GLFW3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)


add_library(ImGui STATIC
	${PROJECT_SOURCE_DIR}/ext/imgui/imgui.cpp ${PROJECT_SOURCE_DIR}/ext/imgui/imgui_draw.cpp ${PROJECT_SOURCE_DIR}/ext/imgui/imgui_tables.cpp ${PROJECT_SOURCE_DIR}/ext/imgui/imgui_widgets.cpp
	${PROJECT_SOURCE_DIR}/ext/imgui/backends/imgui_impl_glfw.cpp
	${PROJECT_SOURCE_DIR}/ext/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(ImGui PRIVATE ${PROJECT_SOURCE_DIR}/ext/imgui)
target_include_directories(ImGui PRIVATE ${PROJECT_SOURCE_DIR}/ext/imgui/backends)

target_link_libraries(ImGui glfw)
target_link_libraries(ImGui OpenGL::GL)

add_subdirectory(src)
