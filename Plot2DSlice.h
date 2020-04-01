#ifndef __plotter_Plot2DSlice__
#define __plotter_Plot2DSlice__

#include "json.hh"
#include "Plot2D.h"

class Generator;

/**
 * @class Plot2DSlice
 * @brief A 2D grid of plots built out of 1D plot slices
 */
class Plot2DSlice : public Plot2D {
public:
  Plot2DSlice(json::Value& c) : Plot2D(c), nslices(-1) {}

  void add(Generator* gen);

private:
  int nslices;
};

#endif  // __plotter_Plot2DSlice__

