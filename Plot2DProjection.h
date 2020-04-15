#ifndef __plotter_Plot2DProjection__
#define __plotter_Plot2DProjection__

#include "json.hh"

class Generator;

class Plot2DProjection : public Plot2D {
public:
  enum Projection { kX, kY };  //!< Direction of projection

  /**
   * Constructor.
   *
   * @param c JSON configuration block
   */
  Plot2DProjection(json::Value& c);

  /**
   * Add a generator to the plot.
   *
   * @param gen The Generator to add
   */
  void add(Generator* gen);

public:
  Projection projection;  //!< What projection to use

private:
  int nslices;  //!< Number of subplot slices
};

#endif  // __plotter_Plot2DProjection__

