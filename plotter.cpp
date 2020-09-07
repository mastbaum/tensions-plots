/**
 * Tensions plotter.
 *
 * A. Mastbaum, D. Cherdack, N. de la Cruz, T. Singh
 */

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include "json.hh"
#include "TStyle.h"

#include "Options.h"
#include "Generator.h"
#include "Plot.h"
#include "Plot1D.h"
#include "Plot2D.h"
#include "Plot2DSlice.h"
#include "Plot2DProjection.h"

int main(int argc, char* argv[]) {
  // Parse CLI arguments
  Options opts(argc, argv);
  if (!opts.valid) return 1;

  // Style
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  // Read JSON config
  std::ifstream config_file(opts.config);
  json::Reader reader(config_file);
  json::Value data;
  reader.getValue(data);
  assert(data.getType() != json::TNULL);

  // Generator configuration
  std::vector<Generator*> gens;
  json::Value& gen_config = data.getMember("generators");
  for (size_t i=0; i<gen_config.getArraySize(); i++) {
    gens.push_back(new Generator(gen_config.getIndex(i)));
  }

  // Plot configuration
  std::vector<Plot*> plots;
  json::Value& plot_config = data.getMember("plots");
  for (size_t i=0; i<plot_config.getArraySize(); i++) {
    json::Value& cfg = plot_config.getIndex(i);
    switch (Plot::getType(cfg)) {
      case Plot::k1D:
        plots.push_back(new Plot1D(cfg));
        break;
      case Plot::k2DSlice:
        plots.push_back(new Plot2DSlice(cfg));
        break;
      case Plot::k2DProjection:
        plots.push_back(new Plot2DProjection(cfg));
        break;
      case Plot::k3D:
      default:
        std::cerr << "Not implemented" << std::endl;
        break;
    }
  }

  // Build overlay plots
  for (Plot* plot : plots) {
    std::cout << plot->sample << std::endl;
    for (Generator* gen : gens) {
      plot->add(gen);
    }

    std::string proj = "";
    if (plot->type == Plot::k2DProjection) {
      proj = ((Plot2DProjection*) plot)->projection == Plot2DProjection::kX ? "_x" : "_y";
    }
    plot->draw(plot->sample + proj + ".C");
    plot->draw(plot->sample + proj + ".pdf");
  }

  return 0;
}

