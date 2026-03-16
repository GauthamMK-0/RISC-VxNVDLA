
CXX      = g++

# SystemC 2.3.4 is installed as a system package
SYSTEMC_INC = /usr/include
SYSTEMC_LIB = /usr/lib/x86_64-linux-gnu

CXXFLAGS = -std=c++17 \
           -I. \
           -I$(SYSTEMC_INC) \
           -Wall -O2

LDFLAGS  = -L$(SYSTEMC_LIB) -lsystemc -lm

NVDLA_SRC = NVDLA/nvdla_top.cpp \
            NVDLA/nvdla_scheduler.cpp \
            NVDLA/nvdla_csb.cpp \
            NVDLA/nvdla_cdma.cpp \
            NVDLA/nvdla_compute.cpp \
            NVDLA/nvdla_dma.cpp \
            NVDLA/nvdla_common.cpp

SRC = CPU/cpu.cpp \
      DMA/dma.cpp \
      Memory/memory.cpp \
      $(NVDLA_SRC) \
      bus/axi_interconnect.cpp \
      IRQ/plic.cpp \
      top/top.cpp

OBJ    = $(SRC:.cpp=.o)
TARGET = soc_sim

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)