#!/bin/zsh

for i in *.{vert,frag}; do
	echo "Compiling shader: " "$i" "->" "${i}.spv"
	glslc "$i" -o "${i}.spv"
	mkdir -p ../bin/shaders
	mv "${i}.spv" ../bin/shaders/
done
