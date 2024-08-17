////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file ITransducerRectangularArray.h.
///
/// NDE file domain path probes[index] / phasedArrayLinear
/// 
/// Declares the ITransducerRectangularArray interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <Equipment/IElementCollection.h>
#include <Equipment/ITransducer.h>

namespace Olympus
{
  namespace Equipment
  {
    /// List of i rectangular transducers.
    class ITransducerRectangularArray;
    using ITransducerRectangularArrayPtr = std::shared_ptr<ITransducerRectangularArray>;
    using ITransducerRectangularArrayConstPtr = std::shared_ptr<const ITransducerRectangularArray>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Ultrasonic transducer including a 2D array of basic transducers
    /// The information of this section is important to allow RayTracing inside the wedge.
    ///
    /// @image html AxisDefinition.png width=30%
    ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class ITransducerRectangularArray : public ITransducer
    {
    public:
      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Destructor
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual ~ITransducerRectangularArray() = default;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a pointer to a collection of the elements of the transducer.
      ///
      /// NDE file domain path probes[index] / phasedArrayLinear / elements
      /// 
      /// @returns A pointer to a collection of the elements of the transducer.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IElementCollectionPtr GetElements() = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets a constant pointer to a collection of the elements of the transducer.
      ///
      /// NDE file domain path probes[index] / phasedArrayLinear / elements
      /// 
      /// @returns A constant pointer to a collection of the elements of the transducer.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual IElementCollectionConstPtr GetElements() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the transducer's elements length.
      /// 
      /// NDE file domain path probes[index] / phasedArrayLinear / primaryAxis / elementLength
      ///
      /// @image html ElementLength.png width=30%
      ///
      /// @returns The transducer's elements length.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetElementLength() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the transducer's elements width.
      ///
      /// NDE file domain path probes[index] / phasedArrayLinear / secondaryAxis / elementLength
      /// 
      /// @image html ElementWidth.png width=30%
      ///
      /// @returns The transducer's elements width.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetElementWidth() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the gap between the transducer's elements on the primary axis. (See IElementOffset.h
      /// for the primary axis definition)
      ///
      /// NDE file domain path probes[index] / phasedArrayLinear / primaryAxis / elementGap
      /// 
      /// @returns The gap between the transducer's elements on the primary axis.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetElementGapOnPrimaryAxis() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the gap between the transducer's elements on the secondary axis. (See IElementOffset.h
      /// for the secondary axis definition)
      ///
      /// NDE file domain path probes[index] / phasedArrayLinear / secondaryAxis / elementGap
      /// 
      /// @returns The gap between the transducer's elements on the primary axis.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual double GetElementGapOnSecondaryAxis() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the number of elements on the transducer on the primary axis. (See IElementOffset.h
      /// for the primary axis definition)
      ///
      /// NDE file domain path probes[index] / phasedArrayLinear / primaryAxis / elementQuantity
      /// 
      /// @returns The number element on the transducer on the primary axis.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual int GetNumberElementOnPrimaryAxis() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the number of elements on the transducer on the secondary axis. (See IElementOffset.h
      /// for the secondary axis definition)
      ///
      /// NDE file domain path probes[index] / phasedArrayLinear / secondaryAxis / elementQuantity
      /// 
      /// @returns The number element on the transducer on the secondary axis.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual int GetNumberElementOnSecondaryAxis() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Gets the total number of elements on the transducer.
      ///
      /// NDE file domain path probes[index] / phasedArrayLinear / elements
      /// 
      /// @returns The total number of elements on the transducer.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual int GetNumberElement() const = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the collection of elements on the transducer.
      ///
      /// @param elements The collection of elements on the transducer.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetElements(IElementCollectionPtr elements) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the transducer's elements length.
      ///
      /// @param elementLength Length of the transducer's elements.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetElementLength(double elementLength) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the transducer's elements width.
      ///
      /// @param elementWidth Width of the transducer's elements.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetElementWidth(double elementWidth) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the elements' gap on the primary axis. (See IElementOffset.h  for the primary axis
      /// definition)
      ///
      /// @param elementGapOnPrimaryAxis The elements' gap on the primary axis.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetElementGapOnPrimaryAxis(double elementGapOnPrimaryAxis) = 0;

      /////////////////////////////////////////////////////////////////////////////////////////////
      /// Sets the elements' gap on the secondary axis. (See IElementOffset.h for the secondary
      /// axis definition)
      ///
      /// @param elementGapOnSecondaryAxis The elements' gap on the secondary axis.
      /////////////////////////////////////////////////////////////////////////////////////////////
      virtual void SetElementGapOnSecondaryAxis(double elementGapOnSecondaryAxis) = 0;
    };
  }
}
