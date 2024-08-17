///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file PartDefinition\ISpecimen.h.
///
/// NDE file domain path: specimens
/// NDE file domain path specimens[index] / plateGeometry / material
/// NDE file domain path specimens[index] / barGeometry / material
/// NDE file domain path specimens[index] / pipeGeometry / material
/// NDE file domain path specimens[index] / weldGeometry / material
/// 
/// Declares the ISpecimen interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <PartDefinition/IMaterial.h>
#include <PartDefinition/IGeometry.h>

namespace Olympus { namespace PartDefinition {

/**
* JSON format location: specimens
*/
class ISpecimen
{
public:
  virtual ~ISpecimen() = default;

  virtual IMaterialPtr GetMaterial() = 0;
  virtual IMaterialConstPtr GetMaterial() const = 0;
  virtual IGeometryPtr GetGeometry() = 0;
  virtual IGeometryConstPtr GetGeometry() const = 0;
};

using ISpecimenPtr = std::shared_ptr<ISpecimen>;
using ISpecimenConstPtr = std::shared_ptr<const ISpecimen>;
}}
