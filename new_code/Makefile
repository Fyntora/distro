CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2
LDFLAGS  := 
osname := "VereLinux"
osversion := "0.1"


TARGET   := distro-builder
SRC := main.cpp \
       builder.cpp \
       modules/rootfs.cpp \
       modules/ubuntu.cpp \
       modules/arch.cpp


OBJ      := $(SRC:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	sudo ./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
