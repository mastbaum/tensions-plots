Tensions Plots
==============
Scripts for plotting data/MC comparisons. These scripts read in the output
of NUISANCE's nuiscomp tool and make overlay comparison plots. Tools are
included for 1D distributions and grids of 1D plots for multi-dimensional
measurements.

Usage
-----
Run `make` to build `plotter`.

To run:

    $ ./plotter -c config/config.json

Note that you'll need to adjust the config file to set paths to your
nuiscomp files.

Configuration
-------------
Plots are configured using a JSON file.

### Generators

The section `generators` contains generator settings with the following fields:

* `filename`: ROOT file containing nuiscomp output histograms
* `title`: Display title for the generator (used in plot legends)
* `color`: A ROOT color number for this generator's histograms

### Plots

The section `plots` contains plot specifications with the following fields:

* `sample` (required): Name of the NUISANCE sample
* `legend_pos` (optional): Legend positioning. Either a string with upper/
  lower and left/right center (one of UL, UC, UR, LL, LC, LR), or an array
  of four numbers setting the (x1, y1, x2, y2) corners.
* `xrange` (optional): A two-element array setting the minimum and maximum x.
* `type`: Type of plot (1D, 2DSlice, 2DProjection), defaults to 1D
* `annotate`: A TLatex annotation

For 2D plots, there are extra parameters:

* `nrows` and `ncols`: Define the 2D plot grid
* `subplot`: Setting that get passed to all the 1D subplots
* `annotate`: An array of TLatex annotations for each plot in the grid

For an example, see `config/config.json`.

