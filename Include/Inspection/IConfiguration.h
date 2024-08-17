///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IConfiguration.h.
///
/// Declares the IConfiguration interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>

namespace Olympus
{
  namespace Inspection
  {
    class IInspectionMethod;
    using IInspectionMethodPtr = std::shared_ptr<IInspectionMethod>;

    class IConfiguration
    {
    public:
      virtual ~IConfiguration() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the underlying inspection method.
      ///
      /// NDE file domain path "dataGroups"[index]
      /// 
      /// @returns The underlying inspection method.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IInspectionMethodPtr GetInspectionMethod() const = 0;
    };

    using IConfigurationPtr = std::shared_ptr<IConfiguration>;
    using IConfigurationConstPtr = std::shared_ptr<const IConfiguration>;
  }
}
