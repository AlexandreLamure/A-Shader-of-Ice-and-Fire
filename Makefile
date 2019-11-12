
SRC = lib/glad/src/glad.c src/init.cc src/mesh.cc src/program.cc src/model.cc src/camera.cc src/fbo.cc src/light.cc src/cubemap.cc src/main.cc

all:
	mkdir -p toto
	g++ --std=c++17 -o toto/bin $(SRC) -lpthread -lglfw -lGL -lGLEW -lm -lassimp -ldl -Ilib/glad/include
