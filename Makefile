app:
	g++ -Wall ./src/cube_renderer.cpp src/glad.c -Iinclude -Isrc -Llib -lglfw3 -lopengl32 -lgdi32 -o app
	./app