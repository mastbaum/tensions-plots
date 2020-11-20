#include <cassert>
#include <string>
#include "json.hh"
#include "TCanvas.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TVirtualPad.h"
#include "Plot.h"
#include "Plot1D.h"
#include "Generator.h"
#include "TPaveText.h"

void Plot1D::AxisRangeX::operator()(TH1D* h) {
  h->GetXaxis()->SetRangeUser(xmin, xmax);
}


Plot1D::LegendPos::LegendPos(json::Value& vpos) {
  if (vpos.getType() == json::TSTRING) {
    // Use a pre-defined position
    std::string spos = vpos.getString();
    if      (spos == "UL") set( true, 0.19, 0.65, 0.70, 0.85);
    else if (spos == "UC") set( true, 0.25, 0.65, 0.75, 0.85);
    else if (spos == "LL") set( true, 0.19, 0.18, 0.70, 0.41);
    else if (spos == "LC") set( true, 0.25, 0.18, 0.75, 0.41);
    else if (spos == "LR") set( true, 0.40, 0.18, 0.89, 0.41);
    else if (spos == "UR") set( true, 0.40, 0.65, 0.89, 0.85);
    else                   set(false, 0.00, 0.00, 0.00, 0.00);
  }
  else if (vpos.getType() == json::TARRAY) {
    // User-specified position
    std::vector<double> lcoord = vpos.toVector<double>();
    set(true, lcoord[0], lcoord[1], lcoord[2], lcoord[3]);
  }
}


void Plot1D::LegendPos::set(bool _draw, float _x1, float _y1, float _x2, float _y2) {
  draw = _draw;
  x1 = _x1;
  y1 = _y1;
  x2 = _x2;
  y2 = _y2;
}


Plot1D::Plot1D(json::Value& c)
    : Plot(c), hdata(nullptr), xranger(nullptr), ymax(-1) {
  // Load settings
  if (c.isMember("xrange")) {
    std::vector<double> range = c.getMember("xrange").toVector<double>();
    assert(range.size() == 2);
    xranger = new AxisRangeX(range[0], range[1]);
  }

  if (c.isMember("ymax")) {
    ymax = c.getMember("ymax").getReal();
  }

  if (c.isMember("xtitle")) {
    xtitle = c.getMember("xtitle").getString();
  }

  if (c.isMember("ytitle")) {
    ytitle = c.getMember("ytitle").getString();
  }

  if (c.isMember("annotate")) {
    annotate = c.getMember("annotate").getString();
  }

  if (c.isMember("data_label")) {
    data_label = c.getMember("data_label").getString();
  }
  else {
    data_label = "Data";
  }

  ytitle_offset = c.isMember("ytitle_offset") ? c.getMember("ytitle_offset").getReal() : 1.25;

  if (c.isMember("legend_pos")) {
    json::Value& vpos = c.getMember("legend_pos");
    lloc = LegendPos(vpos);
  }
}


void Plot1D::add(Generator* gen) {
  // Extract the MC histogram for this generator
  std::string mc = sample + "_MC";
  TH1D* hmc = dynamic_cast<TH1D*>(gen->getHistogram(mc));
  assert(hmc);

  // Build a legend title: name and chi2
  std::string title = gen->title + " (#chi^{2}=" + hmc->GetTitle() + ")";
  hmc->SetTitle(title.c_str());
  lines.push_back(hmc);

  // Set the data once (should be the same in all files)
  if (!hdata) {
    std::string data = sample + "_data";
    hdata = dynamic_cast<TH1D*>(gen->getHistogram(data));
    if (!hdata) {
      data = sample + "_DATA";
      hdata = dynamic_cast<TH1D*>(gen->getHistogram(data));
    }
    if (!hdata) {
      data = sample;
      hdata = dynamic_cast<TH1D*>(gen->getHistogram(data));
    }
    assert(hdata);
    hdata->SetLineColor(kBlack);
  }
}


void Plot1D::scale(float factor) {
  hdata->Scale(factor);

  for (TH1D* line : lines) {
    line->Scale(factor);
  }
}


void Plot1D::draw(std::string filename, TVirtualPad* pad) {
  // Canvas setup
  bool own_pad = false;
  if (!pad) {
    pad = new TCanvas("c", "", 500, 500);
    assert(pad);
    pad->SetLeftMargin(0.18);
    pad->SetTopMargin(0.12);
    pad->SetBottomMargin(0.15);
    own_pad = true;
  }

  pad->cd();

  // Draw the data first
  hdata->SetMarkerStyle(20);
  hdata->SetMarkerSize(1);
  hdata->SetMarkerColor(kBlack);

  hdata->GetXaxis()->SetTitleFont(133);
  hdata->GetXaxis()->SetLabelFont(133);
  hdata->GetXaxis()->SetLabelSize(fontsize);
  hdata->GetXaxis()->SetTitleSize(fontsize);
  hdata->GetXaxis()->SetNdivisions(505);
  hdata->GetXaxis()->SetTitleOffset(1.3);
  hdata->GetXaxis()->CenterTitle(false);

  hdata->GetYaxis()->SetTitleFont(133);
  hdata->GetYaxis()->SetLabelFont(133);
  hdata->GetYaxis()->SetLabelSize(fontsize);
  hdata->GetYaxis()->SetTitleSize(fontsize);
  hdata->GetYaxis()->SetNdivisions(505);
  hdata->GetYaxis()->CenterTitle(false);
  if (type == k1D) {
    hdata->GetYaxis()->SetTitleOffset(ytitle_offset);
  }

  if (xtitle != "") hdata->GetXaxis()->SetTitle(xtitle.c_str());
  if (ytitle != "") hdata->GetYaxis()->SetTitle(ytitle.c_str());

  hdata->Draw("e1");

  if (xranger) {
    (*xranger)(hdata);
  }

  // Automatically set the y axis range to avoid clipping any plot
  bool ymax_auto = ymax < 0;
  if (ymax_auto) {
    ymax = (hdata->GetMaximum() + hdata->GetBinError(hdata->GetMaximumBin())) * 1.05;
  }

  // Set the legend position
  TLegend* l = new TLegend(lloc.x1, lloc.y1, lloc.x2, lloc.y2);

  // Draw legend for data
  l->SetLineWidth(0);
  l->AddEntry(hdata, data_label.c_str());

  // Draw all the MC histograms
  for (TH1D* line : lines) {
    line->Draw("hist same");
    line->SetMarkerSize(0);
    l->AddEntry(line, line->GetTitle());

    if (ymax_auto && line->GetMaximum() > ymax) {
      ymax = line->GetMaximum() * 1.1;
    }
  }

  // Redraw the data on top
  hdata->Draw("e1 same");
  hdata->GetYaxis()->SetRangeUser(0, ymax);
  if (lloc.draw) {
    l->Draw();
  }

  // Add annotation label, if any
  TPaveText* tla = nullptr;
  if (!annotate.empty()) {
    tla = new TPaveText(0.5, 0.8, 0.9, 0.99, "NDC");
    tla->SetTextAlign(33);
    tla->SetTextFont(133);
    tla->SetTextSize(fontsize);
    tla->SetBorderSize(0);
    tla->SetFillColor(0);

    std::stringstream ss(annotate);
    std::string s;
    while (getline(ss, s, ';')) {
      tla->AddText(s.c_str());
    }

    tla->Draw();
  }

  pad->Update();

  // Save the final canvas
  if (filename != "") {
    pad->SaveAs((filename + ".pdf").c_str());
    pad->SaveAs((filename + ".C").c_str());
  }

  // Delete the TCanvas if we own it
  if (own_pad) {
    delete pad;
  }
}

