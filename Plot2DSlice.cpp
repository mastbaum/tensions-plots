#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include "json.hh"
#include "TH1D.h"

#include "Plot.h"
#include "Plot2D.h"
#include "Plot2DSlice.h"
#include "Generator.h"

void Plot2DSlice::add(Generator* gen) {
  // Discover slices
  std::string mc_name = sample + "_MC_Slice";
  std::string data_name = sample + "_data_Slice";
  std::vector<std::string> mc_slice_objs;
  std::vector<std::string> data_slice_objs;

  for (std::string& key : gen->keys) {
    if (key.starts_with(mc_name)) {
      mc_slice_objs.push_back(key);
    }
    if (key.starts_with(data_name)) {
      data_slice_objs.push_back(key);
    }
  }

  assert(mc_slice_objs.size() == data_slice_objs.size());
  size_t nfound = mc_slice_objs.size();

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

  if (!annotate.empty()) {
    assert(nfound == annotate.size());
  }

  // Sort by name
  std::sort(mc_slice_objs.begin(), mc_slice_objs.end());
  std::sort(data_slice_objs.begin(), data_slice_objs.end());

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
      TH1D* h = (TH1D*) gen->getHistogram(data_slice_objs[i]);
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
    TH1D* hmc = dynamic_cast<TH1D*>(gen->getHistogram(mc_slice_objs[i]));
    std::string title = gen->title + " (#chi^{2}=" + chi2_title + ")";
    hmc->SetTitle(title.c_str());
    hmc->SetLineWidth(1);
    plots[i]->lines.push_back(hmc);
  }
}

