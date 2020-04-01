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

