CPPC= g++
CFLAGS = -std=c++17 -O2 -g
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
PROJECT = VulkanGameEngine
DIR = ./bin/

compile: shadersCompile *.cpp 
	$(CPPC) $(CFLAGS) -o $(DIR)$(PROJECT) *.cpp vge/*.cpp $(LDFLAGS)

all: compile test

test:
	cd $(DIR)
	./$(PROJECT)

clean:
	rm -rf $(DIR)/*

shadersCompile:
	cd shaders && ./compile.sh