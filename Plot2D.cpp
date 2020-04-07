#include <cassert>
#include <string>
#include "json.hh"
#include "TCanvas.h"
#include "TVirtualPad.h"
#include "Plot.h"
#include "Plot1D.h"
#include "Plot2D.h"
#include "Generator.h"

Plot2D::Plot2D(json::Value& c)
    : Plot(c), nrows(1), ncols(1), ymax(-1), subplot_config(json::TObject()) {
  // Load settings
  nrows = c.getMember("nrows").getInteger();
  ncols = c.getMember("ncols").getInteger();

  if (c.isMember("subplot")) {
    subplot_config = c.getMember("subplot");
  }

  if (c.isMember("annotate")) {
    annotate = c.getMember("annotate").toVector<std::string>();
  }
}


void Plot2D::draw(std::string filename, TVirtualPad* pad) {
  // Canvas setup
  bool own_pad = false;
  if (!pad) {
    pad = new TCanvas("c", "", 2000, 1500);
    assert(pad);
    pad->SetLeftMargin(0.20);
    pad->SetTopMargin(0.20);
    pad->SetBottomMargin(0.20);
    own_pad = true;
  }

  pad->cd();
  pad->Divide(ncols, nrows, 0, 0);

  // Draw all the subplots
  for (int i=0; i<plots.size(); i++) {
    TVirtualPad* p = pad->cd(i+1);
    if (i > 0) {
      plots[i]->lloc.draw = false;
    }
    plots[i]->draw("", p);
  }

  // Save the final canvas
  if (filename != "") {
    pad->SaveAs(filename.c_str());
  }

  // Delete the canvas if we own it
  if (own_pad) {
    delete pad;
  }
}

