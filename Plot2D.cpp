#include <cassert>
#include <string>
#include "json.hh"
#include "TCanvas.h"
#include "TVirtualPad.h"
#include "Plot.h"
#include "Plot1D.h"
#include "Plot2D.h"
#include "Generator.h"
#include "TLatex.h"
#include "TH1D.h"

Plot2D::Plot2D(json::Value& c)
    : Plot(c), nrows(1), ncols(1), ymax(-1), subplot_config(json::TObject()) {
  // Load settings
  nrows = c.getMember("nrows").getInteger();
  ncols = c.getMember("ncols").getInteger();

  if (c.isMember("xlabel")) {
    xlabel = c.getMember("xlabel").getString();
  }

  if (c.isMember("ylabel")) {
    ylabel = c.getMember("ylabel").getString();
  }

  legend_pad = 1;
  if (c.isMember("legend_pad")) {
    legend_pad = c.getMember("legend_pad").getInteger();
  }

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
    pad->SetLeftMargin(0.22);
    pad->SetTopMargin(0.18);
    pad->SetBottomMargin(0.18);
    pad->SetFillStyle(4000);
    pad->SetFrameFillStyle(0);
    own_pad = true;
  }

  pad->cd();
  pad->Divide(ncols, nrows, 0, 0);

  // Scale
  for (auto plot : plots) {
    plot->scale(scale_factor);
  }

  // Axis labels
  pad->cd(0);
  TLatex* label_x = new TLatex;
  label_x->SetTextFont(133);
  label_x->SetTextAlign(21);
  label_x->SetTextSize(fontsize);
  label_x->DrawLatexNDC(0.5, 0.02, xlabel.c_str());

  TLatex* label_y = new TLatex;
  label_y->SetTextFont(133);
  label_y->SetTextAngle(90);
  label_y->SetTextAlign(23);
  label_y->SetTextSize(fontsize);
  label_y->DrawLatexNDC(0.01, 0.5, ylabel.c_str());

  // Auto-scale the y axis
  float ymax = -999;
  for (int i=0; i<plots.size(); i++) {
    float this_ymax = -999;
    if (plots[i]->ymax > 0) {
      this_ymax = plots[i]->ymax;
    }
    else {
      TH1D* h = plots[i]->hdata;
      for (int j=1; j<h->GetNbinsX()+1; j++) {
        float v = (h->GetBinContent(j) + h->GetBinError(j)) * 1.05;
        this_ymax = std::max(this_ymax, v);
      }
    }
    ymax = std::max(ymax, this_ymax);
  }

  for (int i=0; i<plots.size(); i++) {
    if (plots[i]->ymax > 0) continue;

    plots[i]->ymax = ymax;

    TH1D* h = plots[i]->hdata;
    h->GetYaxis()->SetNoExponent();
    float this_ymax = (h->GetMaximum() + h->GetBinError(h->GetMaximumBin())) * 1.05;

    std::vector<float> scales = { 50, 20, 10, 5, 2 };
    for (float yscale : scales) {
      if (this_ymax * yscale < ymax) {
        h->Scale(yscale);
        for (TH1D* line : plots[i]->lines) {
          line->Scale(yscale);
        }
        plots[i]->annotate += Form(";#times%1.0f", yscale);
        break;
      }
    }
  }

  // Draw all the subplots
  for (int i=0; i<plots.size(); i++) {
    TVirtualPad* p = pad->cd(i+1);
    if (i+1 != legend_pad) {
      plots[i]->lloc.draw = false;
    }
    plots[i]->draw("", p);
    p->SetFillStyle(4000);
    p->SetFrameFillStyle(0);
    p->Update();
    p->Modified();
  }

  // Save the final canvas
  if (filename != "") {
    pad->SaveAs((filename + ".pdf").c_str());
    pad->SaveAs((filename + ".C").c_str());
  }

  // Delete the canvas if we own it
  if (own_pad) {
    delete pad;
  }
}

