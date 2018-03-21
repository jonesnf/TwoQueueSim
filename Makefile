CXX=g++
FLAGS=-Wall -g -std=c++14
TARGET=main

$(TARGET): main.cpp
	$(CXX) $(FLAGS) main.cpp -o main

clean:
	rm $(TARGET)
