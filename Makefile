CPPC= g++
CFLAGS = -std=c++17 -O2 -g
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
PROJECT = VulkanGameEngine
DIR = ./bin/

compile: *.cpp
	$(CPPC) $(CFLAGS) -o $(DIR)$(PROJECT) *.cpp $(LDFLAGS)

all: compile test

test: compile
	$(DIR)$(PROJECT)

clean:
	rm -f $(DIR)/*
