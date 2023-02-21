add_executable(Angaarash
	App.cpp
	Console.cpp
	Calculator.cpp
	Parser.cpp
	Expression.cpp
	Function.cpp
	Token.cpp
	helper_functions.cpp)
target_include_directories(Angaarash PRIVATE "${PROJECT_SOURCE_DIR}/ext/imgui")
target_include_directories(Angaarash PRIVATE "${PROJECT_SOURCE_DIR}/ext/imgui/backends")

target_link_libraries(Angaarash ImGui)
