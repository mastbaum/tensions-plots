#include "generator.h"

int main(int argc, char* argv[]) { 
	gStyle->SetOptStat(0);
	gStyle->SetOptTitle(0);
	std::vector<Generator*> genVector = {
		new Generator("name1", "title1", kBlue, "02atest.root"),
		new Generator("name2", "title2", kRed+1, "10atest.root"),
		new Generator("name3", "title3", kGreen+1, "10btest.root"),
		new Generator("name4", "title4", kViolet, "../nuiscomp_20200103/neut/nuiscomp_neut.root"),
		new Generator("name5", "title5", kOrange, "../nuiscomp_20200103/nuwro/nuiscomp_nuwro.root")
	};
	int myValues[5] ={0,4,8,12,16};
	const char* myLabels[5] = {"GENIE 02a","GENIE 10a","GENIE 10b","NEUT","NuWro"};	
	std::map<std::string, std::pair<TH1*, TH1*>>::iterator myMap;
	for (myMap=genVector[0]->plots.begin(); myMap!=genVector[0]->plots.end(); ++myMap) {
		std::string key = myMap->first;
		std::string stringSearch = "T2K_CC1pip_CH_XSec_2Dpmucosmu_nu_MC";
		if (std::equal(stringSearch.rbegin(), stringSearch.rend(), key.rbegin())){

			std::unique_ptr<TCanvas> plotCanvas(new TCanvas());
			plotCanvas->SetLeftMargin(0.1);
			plotCanvas->SetRightMargin(0.05);
			plotCanvas->SetTopMargin(0.05);
			plotCanvas->SetBottomMargin(.1);
			plotCanvas->SetCanvasSize(1400,600);		
			
			TH1D* histData=(TH1D*) genVector[0]->plots[key].second;
			histData->SetLineWidth(1);
			histData->SetLineColor(kBlack);
			histData->Draw("E1");
			float yMin = 0;
			float yMax = (histData->GetMaximum()+histData->GetBinError(histData->GetMaximumBin())) * 1.05;
			double_t yPos = histData->GetMaximum();
			histData->GetYaxis()->SetRangeUser(yMin,yMax);
			histData->GetXaxis()->SetLabelOffset(0.01);

			std::unique_ptr<TLegend> plotLegend(new TLegend(.75,.15,.925,.4));	
			plotLegend->AddEntry(histData,"Data Points with Error Bars","el");

			for (size_t i=0; i<genVector.size(); i++){                 	
				for (int j=1; j<5; j++){
					TH1D* histMC = (TH1D*) genVector[i]->plots[key].first;	
					histMC->SetLineWidth(2);				 
					histMC->SetLineColor(genVector[i]->color);		
			    		histMC->GetXaxis()->SetRangeUser(myValues[j-1],myValues[j]);					
					histMC->DrawCopy("hist c same");		
					if(j==1){
						plotLegend->AddEntry(histMC,myLabels[i]);
					}
				}
			}
			
			for (int i=1;i<4;i++){
				std::unique_ptr<TLine> sepLine(new TLine(4*i,yMin,4*i,yMax));
				sepLine->SetLineColor(kBlack);
				sepLine->SetLineWidth(1);
				sepLine->SetLineStyle(2);
				sepLine->Draw("same");
			}

			std::vector<std::string> allCosLabels;

			for (int i =1; i<16; i = i +4){
				const char* binLabel = histData->GetXaxis()->GetBinLabel(i);
				
				std::stringstream myStream;
				myStream << binLabel;
				std::string myString = myStream.str();
			


				std::size_t stringSep = myString.find("p");
				std::string binLabelCos = myString.substr(0,stringSep-2);
				std::string labelNew=myString.substr(stringSep,myString.length());	
				
				allCosLabels.push_back(binLabelCos);

				histData->GetXaxis()->SetBinLabel(i,labelNew.c_str());
			
			}
			
			for (int i = 0 ; i<4; i++){
				std::unique_ptr<TPaveText> binLabel(new TPaveText(4*(i+1)-2,yMax*.9,4*(i+1)-.05,yMax*.99));
				binLabel->SetFillColor(0);
				binLabel->SetBorderSize(0);
				
				binLabel->AddText(allCosLabels[i].c_str());
				binLabel->SetTextSize(.03);
				binLabel->Draw("same");
			}
			plotLegend->Draw("same");	



			plotCanvas->Update();				
			char filename[105];
			snprintf(filename, 100, "%s.pdf", key.c_str()); 	
			plotCanvas->SaveAs(filename);	
		}
		else {
	
			std::unique_ptr<TCanvas> plotCanvas(new TCanvas());
			plotCanvas->SetLeftMargin(0.14);
			plotCanvas->SetTopMargin(0.12);
			plotCanvas->SetBottomMargin(.12);

			TH1D* histData = (TH1D*) genVector[0]->plots[key].second;
			histData->SetLineWidth(2);
			histData->SetLineColor(kBlack);
			histData->Draw("E1");
			float yMax = (histData->GetMaximum()+histData->GetBinError(histData->GetMaximumBin())) * 1.05;
			for (size_t i=0; i<genVector.size(); i++) {
				TH1D* histMC = (TH1D*) genVector[i]->plots[key].first;
				histMC->SetLineWidth(2);
				histMC->SetLineColor(genVector[i]->color);
				while (histMC->GetMaximum() > yMax){
					yMax = yMax *1.15;
				}
				histMC->Draw("hist c same");
			
			}

			histData->GetYaxis()->SetRangeUser(0,yMax);
			histData->SetTitle(key.c_str());		
		
		 	plotCanvas->Update();
    			char filename[100];
			snprintf(filename, 100, "%s.pdf", key.c_str());
			plotCanvas->SaveAs(filename);
	}
	}		
	return 0;
}
