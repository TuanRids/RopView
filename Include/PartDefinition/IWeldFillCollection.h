///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file PartDefinition\IWeldFillCollection.h.
///
/// NDE file domain path specimens[index] / weldGeometry / fills[index]
/// 
/// Declares the IWeldFillCollection interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <cstddef>
#include <memory>

namespace Olympus {
  namespace PartDefinition
  {
    class IWeldSection;
    using IWeldSectionPtr = std::shared_ptr<IWeldSection>;
    using IWeldSectionConstPtr = std::shared_ptr<const IWeldSection>;

    class IWeldFillCollection
    {
    public:
      virtual ~IWeldFillCollection() = default;

      virtual std::size_t GetCount() const = 0;
      virtual IWeldSectionConstPtr GetFill(std::size_t) const = 0;
      virtual IWeldSectionPtr GetFill(std::size_t) = 0;
      virtual void AddFill(double height, double angle) = 0;
      virtual void RemoveFill(std::size_t) = 0;
    };

    using IWeldFillCollectionPtr = std::shared_ptr<IWeldFillCollection>;
    using IWeldFillCollectionConstPtr = std::shared_ptr<const IWeldFillCollection>;
  }
}
