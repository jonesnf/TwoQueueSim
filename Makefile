CXX=g++
FLAGS=-Wall -g -std=c++14
TARGET=main

$(TARGET): main.cpp packets.cpp
	$(CXX) $(FLAGS) main.cpp packets.cpp -o main

clean:
	rm $(TARGET)
