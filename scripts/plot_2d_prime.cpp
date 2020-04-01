#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TStyle.h>

int main(int argc, char* argv[]) {
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  for (int i=0; i<6; i++) {
    char name[100];

    // File 1
    TFile f("nuiscomp_genie_g1810a0211a.root");
    assert(f.IsOpen());
  
    TH2D* h_data = (TH2D*) f.Get("MINERvA_CCinc_XSec_2DEavq3_nu_data");
    h_data->SetDirectory(0);
  
    TH1D* h_data_0 = h_data->ProjectionY("h_data_0", i+1, i+1);
    h_data_0->SetLineColor(kBlack);
    h_data_0->GetXaxis()->SetTitle("E_{avail} [GeV]");
    h_data_0->GetYaxis()->SetTitle("d^{2}#sigma/dq_{3}dE_{avail} (cm^{2}/GeV^{2})");

    TH2D* h_mc_g1810a0211a = (TH2D*) f.Get("MINERvA_CCinc_XSec_2DEavq3_nu_MC");
    h_mc_g1810a0211a->SetDirectory(0);
 
    TH1D* h_mc_g1810a0211a_0 = h_mc_g1810a0211a->ProjectionY("h_mc_g1810a0211a_0", i+1, i+1);
    h_mc_g1810a0211a_0->SetLineColor(kRed);

    // File 2
    TFile f2("nuiscomp_genie_g1802a0211a.root");
    assert(f2.IsOpen());

    TH2D* h_mc_g1802a0211a = (TH2D*) f2.Get("MINERvA_CCinc_XSec_2DEavq3_nu_MC");
    h_mc_g1802a0211a->SetDirectory(0);

    TH1D* h_mc_g1802a0211a_0 = h_mc_g1802a0211a->ProjectionY("h_mc_g1802a0211a_0", i+1, i+1);
    h_mc_g1802a0211a_0->SetLineColor(kOrange);

    // File 3
    TFile f3("nuiscomp_genie_g1810b0211a.root");
    assert(f3.IsOpen());

    TH2D* h_mc_g1810b0211a = (TH2D*) f3.Get("MINERvA_CCinc_XSec_2DEavq3_nu_MC");
    h_mc_g1810b0211a->SetDirectory(0);

    TH1D* h_mc_g1810b0211a_0 = h_mc_g1810b0211a->ProjectionY("h_mc_g1810b0211a_0", i+1, i+1);
    h_mc_g1810b0211a_0->SetLineColor(kGreen);

    // File 4
    TFile f4("nuiscomp_neut.root");
    assert(f4.IsOpen());

    TH2D* h_mc_neut = (TH2D*) f4.Get("MINERvA_CCinc_XSec_2DEavq3_nu_MC");
    h_mc_neut->SetDirectory(0);

    TH1D* h_mc_neut_0 = h_mc_neut->ProjectionY("h_mc_neut", i+1, i+1);
    h_mc_neut_0->SetLineColor(kBlue);

    // File 5
    TFile f5("nuiscomp_nuwro.root");
    assert(f5.IsOpen());

    TH2D* h_mc_nuwro = (TH2D*) f5.Get("MINERvA_CCinc_XSec_2DEavq3_nu_MC");
    h_mc_nuwro->SetDirectory(0);

    TH1D* h_mc_nuwro_0 = h_mc_neut->ProjectionY("h_mc_nuwro", i+1, i+1);
    h_mc_nuwro_0->SetLineColor(kViolet);


    // Draw
    TCanvas c1;
    h_data_0->Draw("e1");
    h_mc_g1810a0211a_0->Draw("hist same");
    h_mc_g1802a0211a_0->Draw("hist same");
    h_mc_g1810b0211a_0->Draw("hist same");
    h_mc_neut_0->Draw("hist same");
    h_mc_nuwro_0->Draw("hist same");
    h_data_0->Draw("e1 same");

    TLegend l(0.5, 0.6, 0.88, 0.88);
    if (i==0) {
      l.AddEntry(h_data_0, "Data");
      l.AddEntry(h_mc_g1810a0211a_0, "GENIE G1810a0211a");
      l.AddEntry(h_mc_g1802a0211a_0, "GENIE G1802a0211a");
      l.AddEntry(h_mc_g1810b0211a_0, "GENIE G1810b0211a");
      l.AddEntry(h_mc_neut_0, "NEUT");
      l.AddEntry(h_mc_nuwro_0, "NUWRO");
      l.Draw();
    }

    snprintf(name, 100, "slice_2d_%i.pdf", i);
    c1.SaveAs(name);
  } 

  return 0;
}

