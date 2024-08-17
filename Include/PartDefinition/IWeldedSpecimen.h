///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file PartDefinition\IWeldedSpecimen.h.
///
/// NDE file domain path specimens[index] / weldGeometry 
/// 
/// Declares the IWeldedSpecimen interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <PartDefinition/ISpecimen.h>
#include <PartDefinition/IWeld.h>

namespace Olympus { namespace PartDefinition {

    class IWeldedSpecimen : public ISpecimen
    {
    public:
      virtual IWeldPtr GetWeld() = 0;
      virtual IWeldConstPtr GetWeld() const = 0;
      virtual IWeldPtr CreateWeld(WeldBevel, WeldType) = 0;
    };

    using IWeldedSpecimenPtr = std::shared_ptr<IWeldedSpecimen>;
    using IWeldedSpecimenConstPtr = std::shared_ptr<const IWeldedSpecimen>;
  }
}
