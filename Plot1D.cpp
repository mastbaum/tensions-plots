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

void Plot1D::AxisRangeX::operator()(TH1D* h) {
  h->GetXaxis()->SetRangeUser(xmin, xmax);
}


Plot1D::LegendPos::LegendPos(json::Value& vpos) {
  if (vpos.getType() == json::TSTRING) {
    std::string spos = vpos.getString();
    if      (spos == "UL") set( true, 0.18, 0.65, 0.51, 0.85);
    else if (spos == "UC") set( true, 0.33, 0.65, 0.66, 0.85);
    else if (spos == "LL") set( true, 0.18, 0.15, 0.51, 0.35);
    else if (spos == "LC") set( true, 0.33, 0.15, 0.66, 0.35);
    else if (spos == "LR") set( true, 0.50, 0.15, 0.88, 0.35);
    else if (spos == "UR") set( true, 0.50, 0.65, 0.88, 0.85);
    else                   set(false, 0.00, 0.00, 0.00, 0.00);
  }
  else if (vpos.getType() == json::TARRAY) {
    std::vector<double> lcoord = vpos.toVector<double>();
    set(true, lcoord[1], lcoord[1], lcoord[2], lcoord[3]);
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
  // Configure x range, if set
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

  // Configure legend position, if set
  if (c.isMember("legend_pos")) {
    json::Value& vpos = c.getMember("legend_pos");
    lloc = LegendPos(vpos);
  }
}


void Plot1D::add(Generator* gen) {
  // Extract the MC histogram for this generator
  std::string mc = sample + "_MC";
  TH1D* hmc = dynamic_cast<TH1D*>(gen->getHistogram(mc));

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
    hdata->SetLineColor(kBlack);
    assert(hdata);
  }
}


void Plot1D::draw(std::string filename, TVirtualPad* pad) {
  // Canvas setup
  bool own_pad = false;
  if (!pad) {
    pad = new TCanvas("c", "", 500, 500);
    assert(pad);
    pad->SetLeftMargin(0.14);
    pad->SetTopMargin(0.12);
    pad->SetBottomMargin(0.12);
    own_pad = true;
  }

  pad->cd();

  // Draw the data first
  hdata->GetXaxis()->SetTitleFont(132);
  hdata->GetXaxis()->SetLabelFont(132);
  hdata->GetXaxis()->SetLabelSize(fontsize);
  hdata->GetXaxis()->SetTitleSize(fontsize);
  hdata->GetXaxis()->CenterTitle(false);

  hdata->GetYaxis()->SetTitleFont(132);
  hdata->GetYaxis()->SetLabelFont(132);
  hdata->GetYaxis()->SetLabelSize(fontsize);
  hdata->GetYaxis()->SetTitleSize(fontsize);
  hdata->GetYaxis()->CenterTitle(false);

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

  TLatex* tla = nullptr;
  if (!annotate.empty()) {
    tla = new TLatex;
    tla->SetTextFont(132);
    tla->DrawLatexNDC(0.5, 0.9, annotate.c_str());
  }
  pad->Update();

  // Save the final canvas
  if (filename != "") {
    pad->SaveAs(filename.c_str());
  }

  if (own_pad) {
    delete pad;
  }
}

