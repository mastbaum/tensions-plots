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
#include <TLine.h>
#include <TPaveText.h>

class Generator {
public:
	Generator(std::string name, TString _title, int _color, TString _filename)
		: title(_title), filename(_filename),color(_color) {
	TFile* rootFile = TFile::Open(_filename); //set file location to var myFile
  	TList* histgrms = rootFile->GetListOfKeys(); //getting histograms

	for(int i=0; i<histgrms->GetEntries();i++){   //pulling out all histograms 
		TKey* currHistgrm = (TKey*) histgrms->At(i); //getting location of ith histogram
		std::string histgrmName = currHistgrm->GetName(); //getting name of said histogram
		std::string search = "_MC";
		if (!std::equal(search.rbegin(), search.rend(), histgrmName.rbegin())) continue;//skip over histograms without monte carlo in name 

		TObject* hgObj = rootFile->Get(histgrmName.c_str());
		if (!(hgObj->IsA() == TH1D::Class())) continue;//check to see if it is a 1d histro gram
		TH1* h = (TH1*) hgObj->Clone((histgrmName + "_" +name).c_str()); //what does this line do????
		h->SetDirectory(NULL);

		std::vector<std::string> strings;
		std::istringstream f(histgrmName);
		std::string s;
		while (std::getline(f,s,'_')){
			strings.push_back(s);
		}
		s =""; 
		
		for (size_t i=0; i<strings.size()-1; i++) {
			s+= strings[i] +"_";
		}
		s += "data";
		
		hgObj = rootFile->Get(s.c_str());
		if (!hgObj) {

			s="";
			for (size_t i=0; i<strings.size()-1; i++){
				s += strings[i]+ "_";
		}
		s+= "DATA";
	}
	hgObj = rootFile->Get(s.c_str());


	TH1* hd = (TH1*) hgObj->Clone((s+"_"+name).c_str()); //why do we need a c string???
	hd->SetDirectory(NULL);

	hd->GetXaxis()->SetTitleFont(132);
	hd->GetXaxis()->SetLabelFont(132);
	hd->GetXaxis()->SetLabelSize(0.05);
	hd->GetXaxis()->SetTitleSize(0.05);
	hd->GetYaxis()->SetTitleFont(132);
	hd->GetYaxis()->SetLabelFont(132);
	hd->GetYaxis()->SetLabelSize(0.05);
	hd->GetYaxis()->SetTitleSize(0.05);


	plots[histgrmName] = std::make_pair(h, hd);
	}
	rootFile->Close();
}


	TString title;
	TString filename;
	int color;
	std::map<std::string, std::pair<TH1*, TH1*> > plots;
};

		
