///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file Indication\IPatchPosition.h.
///
/// NDE file domain path / The custom section are not part of the Domain, 
/// 
/// Declares the IPatchPosition interface
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstddef>
#include <memory>

namespace Olympus
{
  namespace Indication
  {
    /// A patch position.
    class IPatchPosition
    {
    public:
      virtual ~IPatchPosition() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the scan position in units from 0 to Ceiling(Scan lenght / Scan resolution).
      /// Corresponds to the PatchBuffer column position.
      ///
      /// @returns The scan position.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetScanPosition() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the scan position in units from 0 to Ceiling(Scan lenght / Scan resolution).
      /// Corresponds to the PatchBuffer column position.
      ///
      /// @param scanPosition The scan position.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetScanPosition(std::size_t scanPosition) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the index position in units from 0 to Ceiling(Index lenght / Index resolution).
      /// Corresponds to the PatchBuffer row position.
      ///
      /// @returns The index position.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual std::size_t GetIndexPosition() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the index position in units from 0 to Ceiling(Index lenght / Index resolution).
      /// Corresponds to the PatchBuffer row position.
      ///
      /// @param indexPosition The index position.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetIndexPosition(std::size_t indexPosition) = 0;
    };

    using IPatchPositionPtr = std::shared_ptr<IPatchPosition>;
    using IPatchPositionConstPtr = std::shared_ptr<const IPatchPosition>;
  }
}
