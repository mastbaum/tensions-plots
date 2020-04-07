CFLAGS=-g -Werror -pedantic $(shell root-config --cflags)
INCLUDE=-I. -I./contrib/fastjson
LFLAGS=$(shell root-config --libs)

SOURCES=Generator.cpp Plot.cpp Plot2D.cpp Plot2DSlice.cpp Options.cpp Plot1D.cpp Plot2DProjection.cpp plotter.cpp

all: plotter

plotter:
	g++ $(CFLAGS) -o plotter $(SOURCES) contrib/fastjson/json.cc $(INCLUDE) $(LFLAGS)

clean:
	rm plotter

