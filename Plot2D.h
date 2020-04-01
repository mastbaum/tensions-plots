#ifndef __plotter_Plot2D__
#define __plotter_Plot2D__

#include <string>
#include "json.hh"
#include "Plot1D.h"

class Generator;
class TVirtualPad;

class Plot2D : public Plot {
public:
  Plot2D(json::Value& c);

  virtual void add(Generator* gen) = 0;

  virtual void draw(std::string filename="", TVirtualPad* pad=nullptr);

public:
  unsigned nrows;  //!< Number of plot grid rows
  unsigned ncols;  //!< Number of plot grid columns
  float ymax;  //!< Maximum y, -1 for auto scale
  std::vector<Plot1D*> plots;  //!< Array of plots for the 2D grid
  Plot1D::LegendPos lloc;  //!< Legend location
  json::Value subplot_config;  //!< Config applied to subplots
  std::vector<std::string> annotate;  //!< Subplot annotations
};

#endif  // __plotter_Plot2D__

