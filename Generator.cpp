#include <algorithm>
#include <cassert>
#include <string>
#include "json.hh"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TList.h"
#include "TString.h"
#include "Generator.h"

Generator::Generator(json::Value& c) : Generator() {
  // Get configuration settings
  title = c.getMember("title").getString();
  color = c.isMember("color") ? c.getMember("color").getInteger() : kBlack;
  std::string filename = c.getMember("filename").getString();
  
  // Load the ROOT file
  tfile = TFile::Open(filename.c_str());
  assert(tfile && tfile->IsOpen());
  TList* tkeys = tfile->GetListOfKeys();

  // Extract a list of keys for later checks
  for (int i=0; i<tkeys->GetEntries(); i++) {
    keys.push_back(tkeys->At(i)->GetName());
  }
}


TH1* Generator::getHistogram(std::string key) {
  // Check if we actually have this object in the file
  if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
    return nullptr;
  }

  // Get the object as a generic TH1
  tfile->cd();
  TH1* ht = (TH1*) tfile->Get(key.c_str());
  assert(ht);

  // Clone the object as a TH(1|2|3)D, for ownership
  TH1* h =  nullptr;;
  if (ht->IsA() == TH1D::Class()) {
    h = (TH1D*) ht->Clone(TString(ht->GetName()) + "_h");
  }
  else if (ht->IsA() == TH2D::Class()) {
    h = (TH2D*) ht->Clone(TString(ht->GetName()) + "_h");
  }
  else if (ht->IsA() == TH3D::Class()) {
    h = (TH3D*) ht->Clone(TString(ht->GetName()) + "_h");
  }
  assert(h);

  h->SetDirectory(nullptr);
  h->SetLineColor(color);
  h->SetLineWidth(2);

  return h;
}


std::string Generator::getChi2String(std::string sample) {
  TH1D* hchi2 = (TH1D*) getHistogram("likelihood_hist");
  TH1D* hndof = (TH1D*) getHistogram("ndof_hist");
  assert(hchi2 && hndof);
  std::string chi2_str, ndof_str;
  for (int i=0; i<hchi2->GetNbinsX()+1; i++) {
    std::string binlabel = hchi2->GetXaxis()->GetBinLabel(i);
    if (binlabel == sample) {
      chi2_str = Form("%1.2f", hchi2->GetBinContent(i));
      break;
    }
  }
  for (int i=0; i<hndof->GetNbinsX()+1; i++) {
    std::string binlabel = hndof->GetXaxis()->GetBinLabel(i);
    if (binlabel == sample) {
      ndof_str = Form("%1.0f", hndof->GetBinContent(i));
      break;
    }
  }
  return chi2_str + "/" + ndof_str;
}

