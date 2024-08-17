////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IElement.h.
///
/// Declares the IElement interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>

namespace Olympus
{
  namespace Equipment
  {
    class IElement;
    using IElementPtr = std::shared_ptr<IElement>;
    using IElementConstPtr = std::shared_ptr<const IElement>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Basic ultrasonic element of a phased array transducer
    /// The information of this section is important to allow RayTracing inside the wedge.
    ///
    /// NDE file domain path "probes"[index] / "phasedArrayLinear" / "elements"[index]
    ///
    /// @image html AxisDefinition.png width=30%
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class IElement
    {
    public:

      virtual ~IElement() = default;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the element name.
      ///
      /// NDE file domain path "probes"[index] / "phasedArrayLinear" / "elements"[index] / "id"
      ///
      /// @return
      /// The element name starting at 1. Refer to the number on the probe casing to identify the elements
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual int GetId() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the primary index.
      ///
      /// NDE file domain path "probes"[index] / "phasedArrayLinear" / "elements"[index] / "primaryIndex"
      /// 
      /// @return
      /// The position of the element on the primary axis of the probe, starting at 1.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual int GetPrimaryIndex() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the secondary index.
      ///
      /// NDE file domain path "probes"[index] / "phasedArrayLinear" / "elements"[index] / "secondaryIndex"
      /// 
      /// @return
      /// The position of the element on the secondary axis of the probe, starting at 1.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual int GetSecondaryIndex() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Query if this element is enabled.
      ///
      /// NDE file domain path "probes"[index] / "phasedArrayLinear" / "elements"[index] / "enabled"
      /// 
      /// @return
      /// True if enabled, false if not.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual bool IsEnabled() const = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets an element name.
      ///
      /// NDE file domain path "probes"[index] / "phasedArrayLinear" / "elements"[index] / "id"
      ///
      /// @param id The element name starting at 1. Refer to the number on the probe casing to identify the elements
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetId(int id) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a primary index.
      ///
      /// NDE file domain path "probes"[index] / "phasedArrayLinear" / "elements"[index] / "primaryIndex"
      /// 
      /// @param primaryIndex The position of the element on the primary axis of the probe, starting at 1.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetPrimaryIndex(int primaryIndex) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets a secondary index.
      ///
      /// NDE file domain path "probes"[index] / "phasedArrayLinear" / "elements"[index] / "secondaryIndex"
      /// 
      /// @param secondaryIndex The position of the element on the secondary axis of the probe, starting at 1.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetSecondaryIndex(int secondaryIndex) = 0;

      ////////////////////////////////////////////////////////////////////////////////////////////////////
      /// Enables or Disables the element.
      ///
      /// NDE file domain path "probes"[index] / "phasedArrayLinear" / "elements"[index] / "enabled"
      /// 
      /// @param isEnabled True to enable the element, false to disable.
      ////////////////////////////////////////////////////////////////////////////////////////////////////
      virtual void Enable(bool isEnabled) = 0;
    };
  }
}
