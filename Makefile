CXX= g++
CXXFLAGS= -g -std=c++20
TARGET= main.exe

main.exe:
	$(CXX) $(CXXFLAGS) -std=c++20 *.cpp cpp_utils/src/*.cpp -o $(TARGET)

clean:
	rm $(TARGET)