CPPC= g++
CFLAGS = -std=c++17 -O2 -g
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
PROJECT = VulkanGameEngine
DIR = ./bin/

default: clean compile

compile: shadersCompile *.cpp 
	$(CPPC) $(CFLAGS) -o $(DIR)$(PROJECT) *.cpp vge/*.cpp $(LDFLAGS)

clean:
	rm -rf $(DIR)/*

shadersCompile:
	cd shaders && ./compile.sh
