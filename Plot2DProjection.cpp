#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include "json.hh"
#include "TH1D.h"
#include "TH2D.h"
#include "Plot.h"
#include "Plot2D.h"
#include "Plot2DProjection.h"
#include "Generator.h"

Plot2DProjection::Plot2DProjection(json::Value& c) : Plot2D(c), nslices(-1) {
  // Load settings
  std::string sproj = c.getMember("projection").getString();

  if (sproj == "x") {
    projection = kX;
  }
  else if (sproj == "y") {
    projection = kY;
  }
  else {
    throw("Unknown 2D projection");
  }
}


void Plot2DProjection::add(Generator* gen) {
  // Load input 2D histograms
  std::string mc_name = sample + "_MC";
  TH2D* mc2d = (TH2D*) gen->getHistogram(mc_name);

  std::string data_name = sample + "_data";
  TH2D* data2d = (TH2D*) gen->getHistogram(data_name);

  assert((mc2d->GetNbinsX() == data2d->GetNbinsX()) &&
         (mc2d->GetNbinsY() == data2d->GetNbinsY()));

  size_t nfound = (projection == kX) ? mc2d->GetNbinsY() : mc2d->GetNbinsX();

  // Make sure the requested grid is big enough
  assert(nfound <= nrows * ncols);

  // Check consistency of slices across generators
  if (nslices < 0) {
    std::cout << sample << ": Found " << nfound << " slices." << std::endl;
    nslices = nfound;
  }
  else {
    assert(nfound == nslices);
  }

  // Make sure we have the right number of annotations (if any)
  if (!annotate.empty()) {
    assert(nfound == annotate.size());
  }
  // Find sample chi2 from NUISANCE
  TH1D* hchi2 = (TH1D*) gen->getHistogram("likelihood_hist");
  assert(hchi2);
  std::string chi2_title;
  for (int i=0; i<hchi2->GetNbinsX()+1; i++) {
    std::string binlabel = hchi2->GetXaxis()->GetBinLabel(i);
    if (binlabel == sample) {
      chi2_title = Form("%1.3f", hchi2->GetBinContent(i));
      break;
    }
  }

  // Populate initial slice plots
  if (plots.empty()) {
    plots.resize(nslices);
    for (size_t i=0; i<nslices; i++) {
      if (!annotate.empty()) {
        json::Value va(annotate[i]);
        subplot_config.setMember("annotate", va);
      }
      plots[i] = new Plot1D(subplot_config);

      std::string name = Form("%s_slice_%lu_h", data2d->GetName(), i);

      TH1D* h;
      if (projection == kX) {
        h = (TH1D*) data2d->ProjectionX(name.c_str(), i+1, i+1);
      }
      else {
        h = (TH1D*) data2d->ProjectionY(name.c_str(), i+1, i+1);
      }

      h->SetLineColor(kBlack);
      h->SetLineWidth(1);
      if (ymax > -1) {
        plots[i]->ymax = ymax;
      }
      plots[i]->hdata = h;
    }
  }

  // Add MC histograms
  for (size_t i=0; i<nslices; i++) {
    std::string name = Form("%s_slice_%lu_h", mc2d->GetName(), i);

    TH1D* hmc;
    if (projection == kX) {
      hmc = (TH1D*) mc2d->ProjectionX(name.c_str(), i+1, i+1);
    }
    else {
      hmc = (TH1D*) mc2d->ProjectionY(name.c_str(), i+1, i+1);
    }

    std::string title = gen->title + " (#chi^{2}=" + chi2_title + ")";
    hmc->SetTitle(title.c_str());
    hmc->SetLineWidth(1);
    plots[i]->lines.push_back(hmc);
  }
}

