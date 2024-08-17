#pragma once
#include <PartDefinition/IMappingUV.h>

namespace Olympus { namespace PartDefinition {

class IPlaneSurface : public IMappingUV
{
public:
  virtual ~IPlaneSurface() = 0;
};

using IPlaneSurfacePtr = std::shared_ptr<IPlaneSurface>;
using IPlaneSurfaceConstPtr = std::shared_ptr<const IPlaneSurface>;
}}

inline Olympus::PartDefinition::IPlaneSurface::~IPlaneSurface() {
}

