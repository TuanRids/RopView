///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Indication\ICustomField.h.
///
/// NDE file domain path / The custom section are not part of the Domain, 
/// they must be put in the root of the file under "APP/Replace_By_Your_App_Name" in the HDF 5 tree
/// 
/// Declares the ICustomField interface
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <string>

namespace Olympus
{
  namespace Indication
  {
    class ICustomField
    {
    public:
      virtual ~ICustomField() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the custom fied name.
      ///
      /// @returns The name.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetName() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a name of the custom field
      ///
      /// @param name The name of the custom field.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetName(const std::wstring& name) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the custom fied value.
      ///
      /// @returns The value.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual const std::wstring& GetValue() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a value of the custom field
      ///
      /// @param value The value of the custom field.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetValue(const std::wstring& name) = 0;

    };

    using ICustomFieldPtr = std::shared_ptr<ICustomField>;
    using ICustomFieldConstPtr = std::shared_ptr<const ICustomField>;
  }
}
