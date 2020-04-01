#include <cassert>
#include <iostream>
#include "json.hh"
#include "Plot.h"
#include "Plot2D.h"
#include "Plot2DProjection.h"
#include "Generator.h"

Plot2DProjection::Plot2DProjection(json::Value& c) : Plot2D(c) {
  std::cerr << "Plot2DProjection not implemented." << std::endl;
  assert(false);
}

void Plot2DProjection::add(Generator* gen) {}

