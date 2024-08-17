///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Indication\ICustomFieldCollection.h
///
/// NDE file domain path / The custom section are not part of the Domain, 
/// they must be put in the root of the file under "APP/Replace_By_Your_App_Name" in the HDF 5 tree
/// 
/// Declares the ICustomFieldCollection interface
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include "ICustomField.h"

namespace Olympus
{
  namespace Indication
  {
    class ICustomFieldCollection
    {
    public:
      virtual ~ICustomFieldCollection() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the number of custom field in the collection
      ///
      /// @returns The custom fields count.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetCount() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Adds a custom field to the custom field Collection
      ///
      /// @param name  The name of the custom field.
      /// @param value The value of the custom field.
      ///
      /// @returns A pointer to an ICustomField.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomFieldPtr Add(const std::wstring& name, const std::wstring& value) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Searches for the custom field with the given name.
      ///
      /// @param name The name of the desired custom field.
      ///
      /// @returns A pointer to the found ICustomField.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomFieldPtr FindCustomField(const std::wstring& name) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Searches for the custom field with the given name.
      ///
      /// @param name The name of the desired custom field.
      ///
      /// @returns The a constant pointer of the found ICustomField.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomFieldConstPtr FindCustomField(const std::wstring& name) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a custom field at a given index
      ///
      /// @param index Zero-based index of the custom field.
      ///
      /// @returns A pointer to the pointed ICustomField.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomFieldPtr GetCustomField(std::size_t index) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a custom field at a given index
      ///
      /// @param index Zero-based index of the custom field.
      ///
      /// @returns A constant pointer to the pointed ICustomField.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ICustomFieldConstPtr GetCustomField(std::size_t index) const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Removes the custom field at a given index
      ///
      /// @param index The index of the custom field to remove.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void Remove(std::size_t index) = 0;
    };

    using ICustomFieldCollectionPtr = std::shared_ptr<ICustomFieldCollection>;
    using ICustomFieldCollectionConstPtr = std::shared_ptr<const ICustomFieldCollection>;
  }
}
