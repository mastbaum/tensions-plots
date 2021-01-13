{
  TStyle* myStyle = new TStyle("MyStyle", "My Root Style");

  // from ROOT plain style
  myStyle->SetCanvasBorderMode(0);
  myStyle->SetPadBorderMode(0);
  myStyle->SetPadColor(0);
  myStyle->SetCanvasColor(0);
  myStyle->SetStatColor(0);
  myStyle->SetPalette(1);
  myStyle->SetStatBorderSize(1);
  myStyle->SetStatFont(132);
  myStyle->SetLegendBorderSize(0);

  // Default canvas positioning
  myStyle->SetCanvasDefH(500);
  myStyle->SetCanvasDefW(500);
  myStyle->SetPadBottomMargin(0.11);
  myStyle->SetPadTopMargin(0.09);
  myStyle->SetPadLeftMargin(0.13);
  myStyle->SetPadRightMargin(0.11);
  myStyle->SetPadTickX(1);
  myStyle->SetPadTickY(1);
  myStyle->SetFrameBorderMode(0);

  // Fonts
  int font = 132;
  myStyle->SetLabelFont(font, "xyz");
  myStyle->SetTitleFont(font, "xyz");
  myStyle->SetTitleSize(0.05, "xyz");
  myStyle->SetLabelSize(0.05, "xyz");
  myStyle->SetTitleOffset(1.4, "y");
  myStyle->SetLegendFont(font);

  gROOT->SetStyle("MyStyle");

  // Color palette
  const Int_t NRGBs = 5;
  const Int_t NCont = 99;
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);

  gROOT->ForceStyle();
}

