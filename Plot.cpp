#include <string>
#include "json.hh"
#include "Plot.h"
#include "Generator.h"

Plot::Plot(json::Value& c) : Plot() {
  // Load settings
  if (c.isMember("sample")) {
    sample = c.getMember("sample").getString();
  }

  scale_factor = 1.0;
  if (c.isMember("scale")) {
    scale_factor = c.getMember("scale").getReal();
  }

  type = getType(c);

  fontsize = type == k1D ? 20 : 45;
  if (c.isMember("fontsize")) {
    fontsize = c.getMember("fontsize").getReal();
  }
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

