#include <string>
#include "json.hh"
#include "Plot.h"
#include "Generator.h"

Plot::Plot(json::Value& c) : Plot() {
  // Load settings
  if (c.isMember("sample")) {
    sample = c.getMember("sample").getString();
  }

  fontsize = 0.05;
  if (c.isMember("fontsize")) {
    fontsize = c.getMember("fontsize").getReal();
  }

  type = getType(c);
}


Plot::PlotType Plot::getType(json::Value& c) {
  if (!c.isMember("type")) return k1D;
  std::string t = c.getMember("type").getString();
  if (t == "1D") return k1D;
  else if (t == "2DSlice") return k2DSlice;
  else if (t == "2DProjection") return k2DProjection;
  else if (t == "3D") return k3D;
  return kUnknown;
}

