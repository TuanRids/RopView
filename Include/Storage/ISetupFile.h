#pragma once
#include <memory>
#include <CustomSection/ICustomSectionCollection.h>
#include <storage/ISetup.h>

namespace Olympus
{
  namespace FileManagement
  {
    class ISetupFile
    {
    public:
      virtual ~ISetupFile() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the setup
      ///
      /// @returns The setup.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ISetupPtr GetSetup() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets custom sections
      ///
      /// @returns The custom sections.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomSectionCollectionPtr GetCustomSections() const = 0;

      /// Updates the setup and custom sections in the file
      virtual void Save() = 0;
    };

    using ISetupFilePtr = std::shared_ptr<ISetupFile>;
    using ISetupFileConstPtr = std::shared_ptr<const ISetupFile>;
  }
}
