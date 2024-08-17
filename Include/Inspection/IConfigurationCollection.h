///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IConfigurationCollection.h.
///
/// Declares the IConfigurationCollection interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <memory>
#include <Inspection/InspectionMethod.h>

namespace Olympus
{
  namespace Inspection
  {
    class IInspectionMethodConventional;
    using IInspectionMethodConventionalPtr = std::shared_ptr<IInspectionMethodConventional>;

    class IInspectionMethodPhasedArray;
    using IInspectionMethodPhasedArrayPtr = std::shared_ptr<IInspectionMethodPhasedArray>;

    class IInspectionMethodTotalFocusing;
    using IInspectionMethodTotalFocusingPtr = std::shared_ptr<IInspectionMethodTotalFocusing>;

    class IConfiguration;
    using IConfigurationPtr = std::shared_ptr<IConfiguration>;
    using IConfigurationConstPtr = std::shared_ptr<const IConfiguration>;

    class IConventionalConfiguration;
    using IConventionalConfigurationPtr = std::shared_ptr<IConventionalConfiguration>;

    class IPhasedArrayConfiguration;
    using IPhasedArrayConfigurationPtr = std::shared_ptr<IPhasedArrayConfiguration>;

    class ITotalFocusingMethodConfiguration;
    using ITotalFocusingMethodConfigurationPtr = std::shared_ptr<ITotalFocusingMethodConfiguration>;

    class IConfigurationCollection
    {
    public:
      virtual ~IConfigurationCollection() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the inspection configuration count
      ///
      /// @returns The count.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetCount() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a configuration
      ///
      /// NDE file domain path "dataGroups"[index]
      /// 
      /// @param index Zero-based index of the configuration.
      ///
      /// @returns The configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IConfigurationPtr GetConfiguration(std::size_t index) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a configuration
      ///
      /// NDE file domain path "dataGroups"[index]
      /// 
      /// @param index Zero-based index of the configuration.
      ///
      /// @returns The configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IConfigurationConstPtr GetConfiguration(std::size_t index) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Searches for a configuration
      ///
      /// NDE file domain path "dataGroups"[index]
      /// 
      /// @param methodName Name of the method.
      ///
      /// @returns The found configuration or a null pointer if not found.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IConfigurationPtr FindConfiguration(const std::wstring& methodName) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Searches for a configuration
      ///
      /// NDE file domain path "dataGroups"[index]
      /// 
      /// @param methodName Name of the method.
      ///
      /// @returns The found configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IConfigurationConstPtr FindConfiguration(const std::wstring& methodName) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Adds a conventional method
      ///
      /// @param inspectionMethod The inspection method.
      ///
      /// @returns An IConventionalConfigurationPtr.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IConventionalConfigurationPtr AddConventional(IInspectionMethodConventionalPtr inspectionMethod) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Adds a phased array method
      ///
      /// @param inspectionMethod The inspection method.
      ///
      /// @returns An IPhasedArrayConfigurationPtr.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IPhasedArrayConfigurationPtr AddPhasedArray(IInspectionMethodPhasedArrayPtr inspectionMethod) = 0;

      virtual ITotalFocusingMethodConfigurationPtr AddTotalFocusingMethod(IInspectionMethodTotalFocusingPtr inspectionMethod) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Removes the configuration at a given index
      ///
      /// @param index Zero-based index of the configuration.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void RemoveConfiguration(std::size_t index) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Removes the configuration described by a given name
      ///
      /// @param name The name.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void RemoveConfiguration(const std::wstring& name) = 0;
    };

    using IConfigurationCollectionPtr = std::shared_ptr<IConfigurationCollection>;
    using IConfigurationCollectionConstPtr = std::shared_ptr<const IConfigurationCollection>;
  }
}
