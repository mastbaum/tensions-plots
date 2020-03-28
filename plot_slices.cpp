#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TStyle.h>

int main(int argc, char* argv[]) {
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  std::vector<std::string> labels = {
    "-1.00 < cos(#theta_{#mu}^{reco}) < -0.50",
    "-1.00 < cos(#theta_{#mu}^{reco}) < -0.50", // FIXME: actual costheta bins
    "-1.00 < cos(#theta_{#mu}^{reco}) < -0.50",
    "-1.00 < cos(#theta_{#mu}^{reco}) < -0.50",
    "-1.00 < cos(#theta_{#mu}^{reco}) < -0.50",
    "-1.00 < cos(#theta_{#mu}^{reco}) < -0.50",
    "-1.00 < cos(#theta_{#mu}^{reco}) < -0.50",
    "-1.00 < cos(#theta_{#mu}^{reco}) < -0.50",
    "-1.00 < cos(#theta_{#mu}^{reco}) < -0.50",
  };

  for (int i=0; i<9; i++) {
    char name[100];

    // File 1
    TFile f("nuiscomp_genie_g1810a0211a.root");
    assert(f.IsOpen());

    snprintf(name, 100, "MicroBooNE_CCInc_XSec_2DPcos_nu_data_Slice%i", i);
    TH1D* h_data_0 = (TH1D*) f.Get(name);
    h_data_0->SetDirectory(0);
    h_data_0->SetLineColor(kBlack);
    h_data_0->GetXaxis()->SetTitle("p_{#mu}^{reco} [GeV]");
    h_data_0->GetYaxis()->SetTitle("d^{2}#sigma/dp_{#mu}^{reco}dcos(#theta_{#mu}^{reco}) (cm^{2}/nucleon/GeV)");

    snprintf(name, 100, "MicroBooNE_CCInc_XSec_2DPcos_nu_MC_Slice%i", i);
    TH1D* h_mc_g1810a0211a_0 = (TH1D*) f.Get(name);
    h_mc_g1810a0211a_0->SetDirectory(0);
    h_mc_g1810a0211a_0->SetLineColor(kRed+1);

    // File 2
    TFile f2("nuiscomp_genie_g1802a0211a.root");
    assert(f2.IsOpen());

    TH1D* h_mc_g1802a0211a_0 = (TH1D*) f2.Get(name);
    h_mc_g1802a0211a_0->SetDirectory(0);
    h_mc_g1802a0211a_0->SetLineColor(kOrange);

    // File 3
    TFile f3("nuiscomp_genie_g1810b0211a.root");
    assert(f3.IsOpen());

    TH1D* h_mc_g1810b0211a_0 = (TH1D*) f3.Get(name);
    h_mc_g1810b0211a_0->SetDirectory(0);
    h_mc_g1810b0211a_0->SetLineColor(kGreen);

    // File 4
    TFile f4("nuiscomp_neut.root");
    assert(f4.IsOpen());

    TH1D* h_mc_neut_0 = (TH1D*) f4.Get(name);
    h_mc_neut_0->SetDirectory(0);
    h_mc_neut_0->SetLineColor(kBlue);

    // File 5
    TFile f5("nuiscomp_nuwro.root");
    assert(f5.IsOpen());

    TH1D* h_mc_nuwro_0 = (TH1D*) f5.Get(name);
    h_mc_nuwro_0->SetDirectory(0);
    h_mc_nuwro_0->SetLineColor(kViolet);

    // ... files: GENIE G1810b0211a, NuWro, NEUT

    // Draw
    TCanvas c1;

    h_data_0->Draw("e1");
    h_mc_g1810a0211a_0->Draw("hist same");
    h_mc_g1802a0211a_0->Draw("hist same");
    h_mc_g1810b0211a_0->Draw("hist same");
    h_mc_neut_0->Draw("hist same");
    h_mc_nuwro_0->Draw("hist same");
    // ... files
    h_data_0->Draw("e1 same");

    TLatex label;
    label.DrawLatexNDC(0.4, 0.65, labels[i].c_str());  // FIXME: Positioning

    TLegend l(0.5, 0.6, 0.88, 0.88);
    if (i == 0) {
      l.AddEntry(h_data_0, "Data");
      l.AddEntry(h_mc_g1810a0211a_0, "GENIE G1810a0211a");
      l.AddEntry(h_mc_g1802a0211a_0, "GENIE G1802a0211a");
      l.AddEntry(h_mc_g1810b0211a_0, "GENIE G1810b0211a");
      l.AddEntry(h_mc_neut_0, "NEUT");
      l.AddEntry(h_mc_nuwro_0, "NUWRO");
      // ... files
      l.Draw();
    }

    snprintf(name, 100, "slice%i.pdf", i);
    c1.SaveAs(name);
  }

  return 0;
}

