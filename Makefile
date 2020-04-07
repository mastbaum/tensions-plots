CFLAGS=-g -Werror -pedantic -lm -ldl -m64 -std=c++14
INCLUDE=-I. -I./contrib/fastjson -I/Users/mastbaum/sw/root-6.14.06/include
LFLAGS=-L/Users/mastbaum/sw/root-6.14.06/lib -lCore -lImt -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lROOTDataFrame -lROOTVecOps -lTree -lTreePlayer -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -lpthread

SOURCES=Generator.cpp Plot.cpp Plot2D.cpp Plot2DSlice.cpp Options.cpp Plot1D.cpp Plot2DProjection.cpp plotter.cpp

all: plotter

plotter:
	g++ $(CFLAGS) -o plotter $(SOURCES) contrib/fastjson/json.cc $(INCLUDE) $(LFLAGS)

clean:
	rm plotter

