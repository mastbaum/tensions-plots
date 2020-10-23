#ifndef __plotter_Plot1D__
#define __plotter_Plot1D__

#include <string>
#include "json.hh"

class Generator;
class TH1D;
class TVirtualPad;

/**
 * @class Plot1D
 * @brief A 1D histogram plot
 */
class Plot1D : public Plot {
public:
  /**
   * @class AxisRangeX
   * @brief Functor to set the x axis range
   */
  class AxisRangeX {
  public:
    /**
     * Constructor.
     *
     * @param _xmin Minimum x range value
     * @param _xmax Maximum x range value
     */
    AxisRangeX(float _xmin, float _xmax) : xmax(_xmax), xmin(_xmin) {}

    /**
     * Set the x axis range
     *
     * @param h Histogram to modify
     */
    void operator()(TH1D* h);

  private:
    float xmax;  //!< Range maximum
    float xmin;  //!< Range minimum
  };

  /**
   * @struct LegendPos
   * @brief TLegend positioning
   */
  class LegendPos {
  public:
    /** Default ctor. */
    LegendPos() : draw(true), x1(0.38), y1(0.63), x2(0.88), y2(0.86) {}

    /**
     * Constructor with explicit settings.
     *
     * @param _draw Draw the legend?
     * @param _x1 Lower x
     * @param _y1 Lower y
     * @param _x2 Upper x
     * @param _y2 Upper y
     */
    LegendPos(bool _draw, float _x1, float _y1, float _x2, float _y2)
        : draw(_draw), x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

    /**
     * Constructor with a JSON configuration
     *
     * @param vpos Position configuration object
     */
    LegendPos(json::Value& vpos);

    /**
     * Set all parameters.
     *
     * @param _draw Draw the legend?
     * @param _x1 Lower x
     * @param _y1 Lower y
     * @param _x2 Upper x
     * @param _y2 Upper y
     */
    void set(bool _draw, float _x1, float _y1, float _x2, float _y2);

    bool draw;  //!< Draw the legend?
    float x1;  //!< Lower x
    float y1;  //!< Lower y
    float x2;  //!< Upper x
    float y2;  //!< Upper y
  };

  /** Default ctor. */
  Plot1D() : Plot(), hdata(nullptr), xranger(nullptr) {}

  /**
   * Constructor.
   *
   * @param c JSON configuration block
   */
  Plot1D(json::Value& c);

  /**
   * Add a generator to the plot.
   *
   * @param gen The Generator to add
   */
  void add(Generator* gen);

  /**
   * Scale by a constant.
   *
   * @param factor The scale factor
   */
  void scale(float factor);

  /**
   * Draw the overlaid histograms.
   *
   * @param filename Output filename for PDF
   */
  void draw(std::string filename="", TVirtualPad* pad=nullptr);

public:
  TH1D* hdata;  //!< Data histogram
  std::vector<TH1D*> lines;  //!< MC histograms
  LegendPos lloc;  //!< Legend location
  double ymax;  //!< Max y range, -1 for auto
  std::string xtitle;  //!< Override x title
  std::string ytitle;  //!< Override y title
  std::string annotate;  //!< Annotation
  std::string data_label;  //!< Data label for legend
  float ytitle_offset;  //!< y axis title offset

private:
  AxisRangeX* xranger;  //!< Axis range adjuster
};

#endif  // __plotter_Plot1D__

