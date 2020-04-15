#ifndef __plotter_Plot__
#define __plotter_Plot__

#include <string>
#include "json.hh"

class Generator;
class TVirtualPad;

/**
 * @class Plot
 * @brief Generic base class for plots.
 */
class Plot {
public:
  /** Plot types. */
  enum PlotType {
    k1D, k2DSlice, k2DProjection, k3D, kUnknown
  };

  /** Default ctor. */
  Plot() : type(kUnknown) {}

  /**
   * Constructor with a JSON configuration.
   *
   * @param c JSON configuration block
   */
  Plot(json::Value& c);

  /** Default dtor */
  virtual ~Plot() {}

  /**
   * Add a generator to the plot.
   *
   * @param gen The Generator to add
   */
  virtual void add(Generator* gen) = 0;

  /**
   * Draw the plot.
   *
   * @param filename Filename for the output plot PDF
   */
  virtual void draw(std::string filename="", TVirtualPad* pad=nullptr) = 0;

  /** Extract the type of plot. */
  static PlotType getType(json::Value& c);

public:
  std::string sample;  //!< NUISANCE sample name
  PlotType type;  //!< Type of plot
  double fontsize;  //!< Label and title size
  double scale_factor;  //!< Scale factor
};

#endif  // __plotter_Plot__

