////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Inspection/IInspectionConfigurationCollection.h
///
/// Declares the IInspectionConfigurationCollection interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <Inspection/IInspectionConfiguration.h>
#include <Inspection/IConfiguration.h>
#include <instrumentation/IAcquisition.h>

namespace Olympus
{
  namespace Inspection
  {

  class IConfiguration;
  using IConfigurationPtr = std::shared_ptr<IConfiguration>;
  using IConfigurationConstPtr = std::shared_ptr<const IConfiguration>;

  
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  /// Gives access to the inspection configurations in this collection.
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  class IInspectionConfigurationCollection
  {
  public:
    virtual ~IInspectionConfigurationCollection() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the count for the number of inspection configurations in this collection.
    ///
    /// @return
    /// The number of inspection configurations.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::size_t GetCount() const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get the interface of one inspection configuration in this set by giving its index in the collection.
    ///
    /// @param index The index of the requested inspection configuration in this collection.
    ///
    /// @return
    /// The interface IInspectionConfiguration of the requested inspection configuration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IInspectionConfigurationPtr GetConfiguration(std::size_t index) = 0;

    virtual IInspectionConfigurationConstPtr GetConfiguration(std::size_t index) const = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Add an inspection configuration to this set
    ///
    /// If the configuration is already in the collection (parameter passed pointing to the same object)
    /// then this function has no effect.
    ///
    /// @param inspectionConfiguration The inspection configuration
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Add(IInspectionConfigurationPtr inspectionConfiguration) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Remove an interface IInspectionConfiguration in this collection by giving its index.
    ///
    /// @param index The position of the IInspectionConfiguration desired to be removed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Remove(std::size_t index) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Add an inspection configuration to this set and chain it back.
    ///
    /// @param trigger  The way each cycle data are triggered.
    /// @param acquisitionRate The cycle firing frequency.
    ///
    /// @return
    /// The chained IInspectionConfiguration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef SWIGCSHARP
    virtual IInspectionConfigurationPtr Add(int trigger, double acquisitionRate) = 0;
#else
    virtual IInspectionConfigurationPtr Add(FiringTrigger trigger, double acquisitionRate) = 0;
#endif

    virtual IConfigurationConstPtr FindConfiguration(const std::wstring& configName) const = 0;

    virtual IConfigurationPtr FindConfiguration(const std::wstring& configName) = 0;
  };

  using IInspectionConfigurationCollectionPtr =
    std::shared_ptr<IInspectionConfigurationCollection>;

  using IInspectionConfigurationCollectionConstPtr =
    std::shared_ptr<const IInspectionConfigurationCollection>;

  }
}
