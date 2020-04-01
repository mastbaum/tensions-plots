#ifndef __plotter_Plot2DProjection__
#define __plotter_Plot2DProjection__

#include "json.hh"

class Generator;

class Plot2DProjection : public Plot2D {
public:
  Plot2DProjection(json::Value& c);

  void add(Generator* gen);
};

#endif  // __plotter_Plot2DProjection__

