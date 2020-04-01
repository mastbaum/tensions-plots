#ifndef __plotter_Options__
#define __plotter_Options__

#include <string>

/**
 * @struct Options
 * @brief Command-line plotting options
 */
struct Options {
  /** Default ctor. */
  Options() : valid(true), aflag(0), bflag(0), config(""), nopt(0) {}

  /**
   * Constructor with CLI arguments
   *
   * @param argc Number of arguments
   * @param argv Argument values
   */
  Options(int argc, char* argv[]);

  bool valid;  //!< Is this configuration valid?
  int aflag;
  int bflag;
  std::string config; //!< Configuration JSON file
  unsigned nopt;  //!< Number of options specified
};

#endif  // __plotter_Options__

