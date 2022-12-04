CXX= g++
CXXFLAGS= -g -std=c++20
TARGET= main.exe

main.exe:
	$(CXX) $(CXXFLAGS) *.cpp cpp_utils/src/*.cpp imgui/*.cpp -o $(TARGET) -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl
#	$(CXX) $(CXXFLAGS) *.cpp cpp_utils/src/*.cpp imgui/*.cpp -o $(TARGET)
#	$(CXX) $(CXXFLAGS) $(pkg-config --cflags glfw3) -o $(TARGET) *.cpp cpp_utils/src/*.cpp imgui/*.cpp $(pkg-config --static --libs glfw3)

clean:
	rm $(TARGET)