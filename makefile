GDB=-ggdb 
GPROF=
CFLAGS=-std=c++11 $(GDB) $(GPROF)
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

ants : optimization.o 
	g++ optimization.o -o optimization $(CFLAGS) $(OPENGL)

optimization.o : optimization.cpp
	g++ -c optimization.cpp $(CFLAGS) $(OPENGL)

clean : 
	rm *.o optimization
