#!/bin/zsh

for i in *.{vert,frag}; do
	echo "Compiling shader: " "$i" "->" "${i}.spv"
	glslc "$i" -o "${i}.spv"
	mv "${i}.spv"../bin/shaders/
done
