///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ICustomSectionCollection.h.
/// 
/// NDE file domain path / The custom section are not part of the Domain, 
/// they must be put in the root of the file under "APP/Replace_By_Your_App_Name" in the HDF 5 tree
///
/// Declares the ICustomSectionCollection interface
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <cstddef>
#include <memory>
#include <string>
#include <CustomSection/ICustomSection.h>

namespace Olympus
{
  namespace FileManagement
  {
    class ICustomSectionCollection
    {
    public:
      virtual ~ICustomSectionCollection() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Searches for the first custom section
      ///
      /// @param name The name.
      ///
      /// @returns The found custom section.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomSectionConstPtr FindCustomSection(const std::wstring& name) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Searches for the first custom section
      ///
      /// @param name The name.
      ///
      /// @returns The found custom section.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomSectionPtr FindCustomSection(const std::wstring& name) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets custom section
      ///
      /// @param index Zero-based index of the.
      ///
      /// @returns The custom section.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomSectionConstPtr GetCustomSection (std::size_t index) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets custom section
      ///
      /// @param index Zero-based index of the.
      ///
      /// @returns The custom section.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomSectionPtr GetCustomSection(std::size_t index) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Inserts a custom section
      ///
      /// @param name The name.
      /// @param size The size.
      /// @param data The data.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomSectionPtr AddCustomSection(const std::wstring& name, std::size_t size) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Removes the custom section with the given name
      ///
      /// @param name The name to remove.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void Remove(const std::wstring& name) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the count of custom sections
      ///
      /// @returns The count of custom sections.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetCount() const = 0;

      /// Resets this object
      virtual void Reset() = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Inserts a custom section
      ///
      /// @param name The name.
      /// @param size The size.
      /// @param data The data.
      /// @param format The format of the data (xls, dxf).
      /// @param usage The usage of the data.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomSectionPtr AddCustomSection(const std::wstring& name, std::size_t size, 
        const std::wstring& format, const std::wstring& usage) = 0;
    };

    using ICustomSectionCollectionPtr = std::shared_ptr<ICustomSectionCollection>;
    using ICustomSectionCollectionConstPtr = std::shared_ptr<const ICustomSectionCollection>;
  }
}
