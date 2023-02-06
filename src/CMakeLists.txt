add_executable(Engaar Engaar.cpp Console.cpp Calculator.cpp Parser.cpp Expression.cpp Function.cpp Token.cpp helper_functions.cpp)
target_include_directories(Engaar PRIVATE "${PROJECT_SOURCE_DIR}/ext/imgui")
target_include_directories(Engaar PRIVATE "${PROJECT_SOURCE_DIR}/ext/imgui/backends")

target_link_libraries(Engaar ImGui)
