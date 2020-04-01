/**
 * 1D histograms
 * 
 * Build: g++ -o tensions19_plots tensions19_plots.cpp `root-config --libs --cflags`
 *
 * ATM, 2020
 */

#include <cassert>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TKey.h>
#include <TLegend.h>
#include <TList.h>
#include <TObject.h>
#include <TString.h>
#include <TStyle.h>

class Generator {
public:
  Generator(std::string name, TString _title, int _color, TString _filename)
      : title(_title), filename(_filename), color(_color) {
    // Extract all MC histograms from the file
    TFile* tfile = TFile::Open(filename);
    assert(tfile && tfile->IsOpen());
    TList* keys = tfile->GetListOfKeys();
    for (int i=0; i<keys->GetEntries(); i++) {
      TKey* key = (TKey*) keys->At(i);
      std::string keyname = key->GetName();
   
      // Only match MC
      std::string search = "_MC";
      if (!std::equal(search.rbegin(), search.rend(), keyname.rbegin())) continue;

      // Only 1D histograms
      TObject* obj = tfile->Get(keyname.c_str());
      assert(obj);
      if (!(obj->IsA() == TH1D::Class())) continue;

      TH1* h = (TH1*) obj->Clone((keyname + "_" + name).c_str());
      h->SetDirectory(NULL);
      h->SetMarkerSize(0);

      // Find the matching data histogram
      std::vector<std::string> strings;
      std::istringstream f(keyname);
      std::string sample_name;
      std::string s;
      while (std::getline(f, s, '_')) {
          strings.push_back(s);
      }
      s = "";
      for (size_t i=0; i<strings.size()-1; i++) {
        s += strings[i] + "_";
        sample_name += strings[i] + (i < strings.size()-2 ? "_" : "");
      }
      s += "data";

      obj = tfile->Get(s.c_str());

      if (!obj) {
        s = "";
        for (size_t i=0; i<strings.size()-1; i++) {
          s += strings[i] + "_";
        }
        s += "DATA";
      }
      obj = tfile->Get(s.c_str());

      assert(obj);
      TH1* hd = (TH1*) obj->Clone((s + "_" + name).c_str());
      hd->SetDirectory(NULL);

      hd->GetXaxis()->SetTitleFont(132);
      hd->GetXaxis()->SetLabelFont(132);
      hd->GetXaxis()->SetLabelSize(0.05);
      hd->GetXaxis()->SetTitleSize(0.05);
      hd->GetXaxis()->CenterTitle(false);
      hd->GetYaxis()->SetTitleFont(132);
      hd->GetYaxis()->SetLabelFont(132);
      hd->GetYaxis()->SetLabelSize(0.05);
      hd->GetYaxis()->SetTitleSize(0.05);
      hd->GetYaxis()->CenterTitle(false);

      // Append chi2 to plot title
      TH1D* hchi2 = (TH1D*) tfile->Get("likedivndof_hist");
      assert(hchi2);
      float c2;
      TString plot_title = title;
      for (int i=0; i<hchi2->GetNbinsX()+1; i++) {
        std::string binlabel = hchi2->GetXaxis()->GetBinLabel(i);
        if (binlabel == sample_name) {
          float c2 = hchi2->GetBinContent(i);
          plot_title = Form("%s (#chi^{2}/dof=%1.1f)", title.Data(), c2);
          break;
        }
      }

      plots[keyname] = { sample_name, h, hd, c2, plot_title };
      std::cout << filename << " " << sample_name << std::endl;

    }
    tfile->Close();
  }

  struct Sample {
    std::string name;
    TH1* hmc;
    TH1* hdata;
    float chi2;
    TString title;
  };

  TString title;  //!< Histogram title
  TString filename;  //!< Name of file containing plots
  int color;  //!< Color for drawing
  std::map<std::string, Sample> plots;  //!< Set of all plots loaded (MC, data)
};


typedef enum { kUL, kUC, kUR, kLL, kLC, kLR } LegendLoc;
struct PlotStyle {
  LegendLoc lloc;
  bool set_xlim;
  float xmin;
  float xmax;
};


int main(int argc, char* argv[]) {
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  // Set up all Generators
  std::vector<Generator*> gens = {
     new Generator("gv3_0", "GENIE v3.00.04a, G1802a0211a", kBlue,    "genie/nuiscomp_genie_g1802a0211a.root")
    ,new Generator("gv3_1", "GENIE v3.00.04a, G1810a0211a", kRed+1,   "genie/nuiscomp_genie_g1810a0211a.root")
    ,new Generator("gv3_2", "GENIE v3.00.04a, G1810b0211a", kGreen+1, "genie/nuiscomp_genie_g1810b0211a.root")
    ,new Generator("nuwro", "NuWro 19.02.1",                kOrange,  "nuwro/nuiscomp_nuwro.root")
    ,new Generator("neut",  "NEUT 5.4.0",                   kViolet,  "neut/nuiscomp_neut.root")
  };

  // Plot style overrides
  std::map<std::string, PlotStyle> styles = {
    { "MicroBooNE_CCInc_XSec_2DPcos_nu",             { kUL, false, 0, 0} },
    { "MINERvA_CC0pi_XSec_1DQ2_TgtCH_nu",            { kUR, false, 0, 0} },
    { "MINERvA_CC0pi_XSec_1Dpt_nu",                  { kUR, false, 0, 0} },
    { "MINERvA_CC0pi_XSec_1Dpz_nu",                  { kUR, false, 0, 0} },
    { "MINERvA_CC0pi_XSec_1DQ2QE_nu",                { kUR, false, 0, 0} },
    { "MINERvA_CC0pi_XSec_1DEnuQE_nu",               { kLR, false, 0, 0} },
    { "MINERvA_CC0pinp_STV_XSec_1Dpmu_nu",           { kUR, false, 0, 0} },
    { "MINERvA_CC0pinp_STV_XSec_1Dthmu_nu",          { kLC, false, 0, 0} },
    { "MINERvA_CC0pinp_STV_XSec_1Dpprot_nu",         { kLL, false, 0, 0} },
    { "MINERvA_CC0pinp_STV_XSec_1Dthprot_nu",        { kUL, false, 0, 0} },
    { "MINERvA_CC0pinp_STV_XSec_1Dpnreco_nu",        { kUR, false, 0, 0} },
    { "MINERvA_CC0pinp_STV_XSec_1Ddalphat_nu",       { kUL, false, 0, 0} },
    { "MINERvA_CC0pinp_STV_XSec_1Ddpt_nu",           { kUR, false, 0, 0} },
    { "MINERvA_CC0pinp_STV_XSec_1Ddphit_nu",         { kUR, false, 0, 0} },
    { "MINERvA_CCQE_XSec_1DQ2_nu",                   { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1DTpi_nu",                { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1DTpi_nu_20deg",          { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1DTpi_nu_fluxcorr",       { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1DTpi_nu_20deg_fluxcorr", { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1Dth_nu",                 { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1Dth_nu_20deg",           { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1Dth_nu_fluxcorr",        { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1Dth_nu_20deg_fluxcorr",  { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1DTpi_nu_2017",           { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1Dth_nu_2017",            { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1Dpmu_nu_2017",           { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1Dthmu_nu_2017",          { kLC, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1DQ2_nu_2017",            { kUR, false, 0, 0} },
    { "MINERvA_CC1pip_XSec_1DEnu_nu_2017",           { kLC, false, 0, 0} },
    { "T2K_CC0pinp_STV_XSec_1Ddpt_nu",               { kUR, false, 0, 0} },
    { "T2K_CC0pinp_STV_XSec_1Ddphit_nu",             { kUR, false, 0, 0} },
    { "T2K_CC0pinp_STV_XSec_1Ddat_nu",               { kLC, false, 0, 0} },
    { "T2K_CC1pip_CH_XSec_2Dpmucosmu_nu",            { kUR, false, 0, 0} },
    { "T2K_CC1pip_CH_XSec_1Dppi_nu",                 { kUR,  true, 0, 3} },
    { "T2K_CC1pip_CH_XSec_1Dthpi_nu",                { kUR, false, 0, 0} },
    { "T2K_CC1pip_CH_XSec_1Dthmupi_nu",              { kUR, false, 0, 0} },
    { "T2K_CC1pip_CH_XSec_1DQ2_nu",                  { kUR, false, 0, 0} },
    { "T2K_CC1pip_CH_XSec_1DAdlerPhi_nu",            { kLL, false, 0, 0} },
    { "T2K_CC1pip_CH_XSec_1DCosThAdler_nu",          { kUL, false, 0, 0} }
  };

  // Loop over plots and draw
  std::map<std::string, Generator::Sample>::iterator it;
  for (it=gens[0]->plots.begin(); it!=gens[0]->plots.end(); ++it) {
    std::string key = it->first;

    Generator::Sample& main_sample = gens[0]->plots[key];
    PlotStyle& main_style = styles[main_sample.name];

    TCanvas* c = new TCanvas();
    c->SetLeftMargin(0.14);
    c->SetTopMargin(0.12);
    c->SetBottomMargin(0.12);
    TLegend* l;

    if (main_style.lloc == kUL) l = new TLegend(0.18, 0.65, 0.51, 0.85);
    if (main_style.lloc == kUC) l = new TLegend(0.33, 0.65, 0.66, 0.85);
    if (main_style.lloc == kUR) l = new TLegend(0.50, 0.65, 0.88, 0.85);
    if (main_style.lloc == kLL) l = new TLegend(0.18, 0.15, 0.51, 0.35);
    if (main_style.lloc == kLC) l = new TLegend(0.33, 0.15, 0.66, 0.35);
    if (main_style.lloc == kLR) l = new TLegend(0.50, 0.15, 0.88, 0.35);
    l->SetBorderSize(0);

    TH1D* hdata = (TH1D*) gens[0]->plots[key].hdata;
    hdata->SetLineWidth(2);
    hdata->SetLineColor(kBlack);
    hdata->Draw("e1");
    float ymax = (hdata->GetMaximum() + hdata->GetBinError(hdata->GetMaximumBin())) * 1.05;

    for (size_t i=0; i<gens.size(); i++) {
      Generator::Sample& sample = gens[i]->plots[key];
      TH1D* hmc = (TH1D*) sample.hmc;
      hmc->SetLineWidth(2);
      hmc->SetLineColor(gens[i]->color);
      if (hmc->GetMaximum() > ymax) {
        ymax = hmc->GetMaximum() * 1.1;
      }
      hmc->Draw("hist same");
      l->AddEntry(hmc, sample.title);
    }

    hdata->GetYaxis()->SetRangeUser(0, ymax);
    hdata->SetTitle(key.c_str());
    hdata->Draw("e1 same");

    l->Draw();

    if (styles.find(main_sample.name) != styles.end()) {
      PlotStyle& style = styles[main_sample.name];
      if (style.set_xlim) {
        hdata->GetXaxis()->SetRangeUser(style.xmin, style.xmax);
      }
    }

    c->Update();

    char filename[100];
    snprintf(filename, 100, "%s.pdf", key.c_str());
    c->SaveAs(filename);
  }

  return 0;
}

