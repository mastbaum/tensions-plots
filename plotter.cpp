/**
 * Tensions plotter.
 *
 * A. Mastbaum, D. Cherdack, N. de la Cruz, T. Singh
 */

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h>
#include "json.hh"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TList.h"
#include "TString.h"
#include "TStyle.h"
#include "TVirtualPad.h"

/**
 * @struct Options
 * @brief Command-line plotting options
 */
struct Options {
  /** Default ctor. */
  Options() : valid(true), aflag(0), bflag(0), config(""), nopt(0) {}

  /**
   * Constructor with CLI arguments
   *
   * @param argc Number of arguments
   * @param argv Argument values
   */
  Options(int argc, char* argv[]);

  bool valid;  //!< Is this configuration valid?
  int aflag;
  int bflag;
  std::string config; //!< Configuration JSON file
  unsigned nopt;  //!< Number of options specified
};


/**
 * @class Generator
 * @brief A set of NUISANCE comparisons
 */
class Generator {
public:
  /** Default ctor. */
  Generator() : color(kBlack) {}

  /**
   * Constructor with a JSON configuration.
   *
   * @param c JSON configuration block
   */
  Generator(json::Value& c) : Generator() {
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

  /**
   * Get a histogram object out of the file.
   *
   * @param key Name of the object
   * @returns Histogram as a generic TH1*
   */
  TH1* getHistogram(std::string key) {
    // Check if we actually have this object in the file
    if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
      return nullptr;
    }

    tfile->cd();
    TH1* ht = (TH1*) tfile->Get(key.c_str());
    assert(ht);

    // Get the object as a TH(1|2|3)D
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

public:
  std::string title;  //!< Generator display title
  int color;  //!< Line color
  std::vector<std::string> keys;  //!< List of available keys

private:
  TFile* tfile;  //!< ROOT file (nuiscomp output)
};


/**
 * @class Plot
 * @brief Generic base class for plots.
 */
class Plot {
public:
  /** Plot types. */
  enum PlotType {
    k1D, k2DSlice, k2DProjection, k3D, kUnknown
  };

  /** Default ctor. */
  Plot() : type(kUnknown) {}

  /**
   * Constructor with a JSON configuration.
   *
   * @param c JSON configuration block
   */
  Plot(json::Value& c) : Plot() {
    if (c.isMember("sample")) {
      sample = c.getMember("sample").getString();
    }

    fontsize = 0.05;
    if (c.isMember("fontsize")) {
      fontsize = c.getMember("fontsize").getReal();
    }

    type = getType(c);
  }

  /** Default dtor */
  virtual ~Plot() {}

  /**
   * Add a generator to the plot.
   *
   * @param gen The Generator to add
   */
  virtual void add(Generator* gen) = 0;

  /**
   * Draw the plot.
   *
   * @param filename Filename for the output plot PDF
   */
  virtual void draw(std::string filename="", TVirtualPad* pad=nullptr) = 0;

  /** Extract the type of plot. */
  static PlotType getType(json::Value& c) {
    if (!c.isMember("type")) return k1D;
    std::string t = c.getMember("type").getString();
    if (t == "1D") return k1D;
    else if (t == "2DSlice") return k2DSlice;
    else if (t == "2DProjection") return k2DProjection;
    else if (t == "3D") return k3D;
    return kUnknown;
  }

public:
  std::string sample;  //!< NUISANCE sample name
  PlotType type;  //!< Type of plot
  double fontsize;  //!< Label and title size
};


/**
 * @class Plot1D
 * @brief A 1D histogram plot
 */
class Plot1D : public Plot {
public:
  /**
   * @class AxisRangeX
   * @brief Functor to set the x axis range
   */
  class AxisRangeX {
  public:
    /**
     * Constructor.
     *
     * @param _xmin Minimum x range value
     * @param _xmax Maximum x range value
     */
    AxisRangeX(float _xmin, float _xmax) : xmax(_xmax), xmin(_xmin) {}

    /**
     * Set the x axis range
     *
     * @param h Histogram to modify
     */
    void operator()(TH1D* h) {
      h->GetXaxis()->SetRangeUser(xmin, xmax);
    }

  private:
    float xmax;  //!< Range maximum
    float xmin;  //!< Range minimum
  };

  /**
   * @struct LegendPos
   * @brief TLegend positioning
   */
  class LegendPos {
  public:
    LegendPos() : draw(true), x1(0.50), y1(0.65), x2(0.88), y2(0.85) {}

    LegendPos(bool _draw, float _x1, float _y1, float _x2, float _y2)
        : draw(_draw), x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}

    LegendPos(json::Value& vpos) {
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

    void set(bool _draw, float _x1, float _y1, float _x2, float _y2) {
      draw = _draw;
      x1 = _x1;
      y1 = _y1;
      x2 = _x2;
      y2 = _y2;
    }

    bool draw;  //!< Draw the legend?
    float x1;  //!< Lower x
    float y1;  //!< Lower y
    float x2;  //!< Upper x
    float y2;  //!< Upper y
  };

  /** Default ctor. */
  Plot1D() : Plot(), hdata(nullptr), xranger(nullptr) {}

  /**
   * Constructor.
   *
   * @param c JSON configuration block
   */
  Plot1D(json::Value& c) : Plot(c), hdata(nullptr), xranger(nullptr), ymax(-1) {
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

  /**
   * Add a generator to the plot.
   *
   * @param gen The Generator to add
   */
  void add(Generator* gen) {
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

  /**
   * Draw the overlaid histograms.
   *
   * @param filename Output filename for PDF
   */
  void draw(std::string filename="", TVirtualPad* pad=nullptr) {
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

public:
  TH1D* hdata;  //!< Data histogram
  std::vector<TH1D*> lines;  //!< MC histograms
  LegendPos lloc;  //!< Legend location
  double ymax;  //!< Max y range, -1 for auto
  std::string xtitle;  //!< Override x title
  std::string ytitle;  //!< Override y title
  std::string annotate;  //!< Annotation

private:
  AxisRangeX* xranger;  //!< Axis range adjuster
};


class Plot2D : public Plot {
public:
  Plot2D(json::Value& c) : Plot(c), nrows(1), ncols(1), ymax(-1) {
    nrows = c.getMember("nrows").getInteger();
    ncols = c.getMember("ncols").getInteger();

    if (c.isMember("subplot")) {
      subplot_config = c.getMember("subplot");
    }

    if (c.isMember("annotate")) {
      annotate = c.getMember("annotate").toVector<std::string>();
    }
  }

  virtual void add(Generator* gen) = 0;

  virtual void draw(std::string filename="", TVirtualPad* pad=nullptr) {
    // Canvas setup
    bool own_pad = false;
    if (!pad) {
      pad = new TCanvas("c", "", 2000, 1500);
      assert(pad);
      pad->SetLeftMargin(0.20);
      pad->SetTopMargin(0.20);
      pad->SetBottomMargin(0.20);
      own_pad = true;
    }

    pad->cd();
    pad->Divide(nrows, ncols, 0, 0);

    for (int i=0; i<plots.size(); i++) {
      TVirtualPad* p = pad->cd(i+1);
      if (i > 0) {
        plots[i]->lloc.draw = false;
      }
      plots[i]->draw("", p);
    }

    // Save the final canvas
    if (filename != "") {
      pad->SaveAs(filename.c_str());
    }

    if (own_pad) {
      delete pad;
    }
  }

public:
  unsigned nrows;  //!< Number of plot grid rows
  unsigned ncols;  //!< Number of plot grid columns
  float ymax;  //!< Maximum y, -1 for auto scale
  std::vector<Plot1D*> plots;  //!< Array of plots for the 2D grid
  Plot1D::LegendPos lloc;  //!< Legend location
  json::Value subplot_config;  //!< Config applied to subplots
  std::vector<std::string> annotate;  //!< Subplot annotations
};


/**
 * @class Plot2DSlice
 * @brief A 2D grid of plots built out of 1D plot slices
 */
class Plot2DSlice : public Plot2D {
public:
  Plot2DSlice(json::Value& c) : Plot2D(c), nslices(-1) {}

  void add(Generator* gen) {
    // Discover slices
    std::string mc_name = sample + "_MC_Slice";
    std::string data_name = sample + "_data_Slice";
    std::vector<std::string> mc_slice_objs;
    std::vector<std::string> data_slice_objs;

    for (std::string& key : gen->keys) {
      if (key.starts_with(mc_name)) {
        mc_slice_objs.push_back(key);
      }
      if (key.starts_with(data_name)) {
        data_slice_objs.push_back(key);
      }
    }

    assert(mc_slice_objs.size() == data_slice_objs.size());
    size_t nfound = mc_slice_objs.size();

    // Make sure the requested grid is big enough
    assert(nfound <= nrows * ncols);

    // Check consistency of slices across generators
    if (nslices < 0) {
      std::cout << sample << ": Found " << nfound << " slices." << std::endl;
      nslices = nfound;
    }
    else {
      assert(nfound == nslices);
    }

    if (!annotate.empty()) {
      assert(nfound == annotate.size());
    }

    // Sort by name
    std::sort(mc_slice_objs.begin(), mc_slice_objs.end());
    std::sort(data_slice_objs.begin(), data_slice_objs.end());

    // Find sample chi2 from NUISANCE
    TH1D* hchi2 = (TH1D*) gen->getHistogram("likelihood_hist");
    assert(hchi2);
    std::string chi2_title;
    for (int i=0; i<hchi2->GetNbinsX()+1; i++) {
      std::string binlabel = hchi2->GetXaxis()->GetBinLabel(i);
      if (binlabel == sample) {
        chi2_title = Form("%1.3f", hchi2->GetBinContent(i));
        break;
      }
    }

    // Populate initial slice plots
    if (plots.empty()) {
      plots.resize(nslices);
      for (size_t i=0; i<nslices; i++) {
        if (!annotate.empty()) {
          json::Value va(annotate[i]);
          subplot_config.setMember("annotate", va);
        }
        plots[i] = new Plot1D(subplot_config);
        TH1D* h = (TH1D*) gen->getHistogram(data_slice_objs[i]);
        h->SetLineColor(kBlack);
        h->SetLineWidth(1);
        if (ymax > -1) {
          plots[i]->ymax = ymax;
        }
        plots[i]->hdata = h;
      }
    }

    // Add MC histograms
    for (size_t i=0; i<nslices; i++) {
      TH1D* hmc = dynamic_cast<TH1D*>(gen->getHistogram(mc_slice_objs[i]));
      std::string title = gen->title + " (#chi^{2}=" + chi2_title + ")";
      hmc->SetTitle(title.c_str());
      hmc->SetLineWidth(1);
      plots[i]->lines.push_back(hmc);
    }
  }

private:
  int nslices;
};


class Plot2DProjection : public Plot2D {
public:
  Plot2DProjection(json::Value& c) : Plot2D(c) {
    std::cerr << "Plot2DProjection not implemented." << std::endl;
    assert(false);
  }

  void add(Generator* gen) {}

private:
};


int main(int argc, char* argv[]) {
  // Parse CLI arguments
  Options opts(argc, argv);
  if (!opts.valid) return 1;

  // Style
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  // Read JSON config
  std::ifstream config_file(opts.config);
  json::Reader reader(config_file);
  json::Value data;
  reader.getValue(data);
  assert(data.getType() != json::TNULL);

  // Generator configuration
  std::vector<Generator*> gens;
  json::Value& gen_config = data.getMember("generators");
  for (size_t i=0; i<gen_config.getArraySize(); i++) {
    gens.push_back(new Generator(gen_config.getIndex(i)));
  }

  // Plot configuration
  std::vector<Plot*> plots;
  json::Value& plot_config = data.getMember("plots");
  for (size_t i=0; i<plot_config.getArraySize(); i++) {
    json::Value& cfg = plot_config.getIndex(i);
    switch (Plot::getType(cfg)) {
      case Plot::k1D:
        plots.push_back(new Plot1D(cfg));
        break;
      case Plot::k2DSlice:
        plots.push_back(new Plot2DSlice(cfg));
        break;
      case Plot::k2DProjection:
        plots.push_back(new Plot2DProjection(cfg));
        break;
      case Plot::k3D:
      default:
        std::cerr << "Not implemented" << std::endl;
        break;
    }
  }

  // Build overlay plots
  for (Plot* plot : plots) {
    std::cout << plot->sample << std::endl;
    for (Generator* gen : gens) {
      plot->add(gen);
    }

    plot->draw(plot->sample + ".pdf");
  }

  return 0;
}


Options::Options(int argc, char* argv[]) : Options() {
  int c;
  while ((c = getopt(argc, argv, "abc:")) != -1) {
    switch (c) {
      case 'a':
        aflag = 1;
        break;
      case 'b':
        bflag = 1;
        break;
      case 'c':
        config = optarg;
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if(isprint(optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        valid = false;
        break;
    }
  }

  nopt = optind;
}

