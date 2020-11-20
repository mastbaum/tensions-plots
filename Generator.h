#ifndef __plotter_Generator__
#define __plotter_Generator__

#include <string>
#include "json.hh"
#include "TColor.h"

class TFile;
class TH1;

/**
 * @class Generator
 * @brief A set of NUISANCE comparisons
 */
class Generator {
public:
  /** Default ctor. */
  Generator() : color(kBlack) {}

  /**
   * Constructor with a JSON configuration.
   *
   * @param c JSON configuration block
   */
  Generator(json::Value& c);

  /**
   * Get a histogram object out of the file.
   *
   * @param key Name of the object
   * @returns Histogram as a generic TH1*
   */
  TH1* getHistogram(std::string key);

  /**
   * Get the chi2/ndof as a string.
   *
   * @param key Name of the sample
   * @returns "chi2/ndof" as a string
   */
  std::string getChi2String(std::string sample);

public:
  std::string title;  //!< Generator display title
  int color;  //!< Line color
  std::vector<std::string> keys;  //!< List of available keys

private:
  TFile* tfile;  //!< ROOT file (nuiscomp output)
};

#endif  // __plotter_Generator__

