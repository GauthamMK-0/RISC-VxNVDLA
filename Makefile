
CXX = g++
CXXFLAGS = -std=c++17 -I/usr/local/systemc-2.3.3/include -Wall -O2
LDFLAGS = -L/usr/local/systemc-2.3.3/lib-linux64 -lsystemc -lm

SRC = $(wildcard CPU/*.cpp NVDLA/*.cpp DMA/*.cpp Memory/*.cpp IRQ/*.cpp top/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = soc_sim

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)