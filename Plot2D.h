#ifndef __plotter_Plot2D__
#define __plotter_Plot2D__

#include <string>
#include "json.hh"
#include "Plot1D.h"

class Generator;
class TVirtualPad;

/**
 * @class Plot2D
 * @brief Base class for 2D plot grids
 *
 * A Plot2D consists of a set of Plot1Ds that are drawn in a grid. It is up
 * to subclasses to populate the Plot1Ds given some form of 2D input in their
 * implementation of `add`, but then the `draw` function can be shared.
 */
class Plot2D : public Plot {
public:
  /**
   * Constructor.
   *
   * @param c JSON configuration block
   */
  Plot2D(json::Value& c);

  /**
   * Add a generator to the plot.
   *
   * @param gen The Generator to add
   */
  virtual void add(Generator* gen) = 0;

  /**
   * Draw the overlaid histograms.
   *
   * @param filename Output filename for PDF
   */
  virtual void draw(std::string filename="", TVirtualPad* pad=nullptr);

public:
  unsigned nrows;  //!< Number of plot grid rows
  unsigned ncols;  //!< Number of plot grid columns
  int legend_pad;  //!< Which pad ID to draw the legend
  float ymax;  //!< Maximum y, -1 for auto scale
  std::string xlabel;  //!< x axis label
  std::string ylabel;  //!< y axis label
  std::vector<Plot1D*> plots;  //!< Array of plots for the 2D grid
  Plot1D::LegendPos lloc;  //!< Legend location
  json::Value subplot_config;  //!< Config applied to subplots
  std::vector<std::string> annotate;  //!< Subplot annotations
};

#endif  // __plotter_Plot2D__

