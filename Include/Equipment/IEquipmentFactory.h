////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file IEquipmentFactory.h.
///
/// Declares the IEquipmentFactory interface.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <string>
#include <Inspection/BeamSet.h>
#include <Equipment/IElementCollection.h>
#include <instrumentation/IDeviceInfo.h>

namespace Olympus { namespace Equipment {

  class IAcquisitionUnit;
  using IAcquisitionUnitPtr = std::shared_ptr<IAcquisitionUnit>;
  using IAcquisitionUnitConstPtr = std::shared_ptr<const IAcquisitionUnit>;

  class IEncoderSensor;
  using IEncoderSensorPtr = std::shared_ptr<IEncoderSensor>;
  using IEncoderSensorConstPtr = std::shared_ptr<const IEncoderSensor>;

  class IMultiElementDualConnector;
  using IMultiElementDualConnectorPtr = std::shared_ptr<IMultiElementDualConnector>;
  using IMultiElementDualConnectorConstPtr = std::shared_ptr<const IMultiElementDualConnector>;

  class IMultiElementSingleConnector;
  using IMultiElementSingleConnectorPtr = std::shared_ptr<IMultiElementSingleConnector>;
  using IMultiElementSingleConnectorConstPtr = std::shared_ptr<const IMultiElementSingleConnector>;

  class IProbeDualRectangularArray;
  using IProbeDualRectangularArrayPtr = std::shared_ptr<IProbeDualRectangularArray>;
  using IProbeDualRectangularArrayConstPtr = std::shared_ptr<const IProbeDualRectangularArray>;

  class IProbeRectangularArray;
  using IProbeRectangularArrayPtr = std::shared_ptr<IProbeRectangularArray>;
  using IProbeRectangularArrayConstPtr = std::shared_ptr<const IProbeRectangularArray>;

  class IProbeConventional;
  using IProbeConventionalPtr = std::shared_ptr<IProbeConventional>;
  using IProbeConventionalConstPtr = std::shared_ptr<const IProbeConventional>;

  class ITransducerConventionalRectangular;
  using ITransducerConventionalRectangularPtr = std::shared_ptr<ITransducerConventionalRectangular>;
  using ITransducerConventionalRectangularConstPtr = std::shared_ptr<const ITransducerConventionalRectangular>;

  class ITransducerConventionalRound;
  using ITransducerConventionalRoundPtr = std::shared_ptr<ITransducerConventionalRound>;
  using ITransducerConventionalRoundConstPtr = std::shared_ptr<const ITransducerConventionalRound>;

  class ITransducerRectangularArray;
  using ITransducerRectangularArrayPtr = std::shared_ptr<ITransducerRectangularArray>;
  using ITransducerRectangularArrayConstPtr = std::shared_ptr<const ITransducerRectangularArray>;

  class IWedgeConventional;
  using IWedgeConventionalPtr = std::shared_ptr<IWedgeConventional>;
  using IWedgeConventionalConstPtr = std::shared_ptr<const IWedgeConventional>;

  class IWedgeDualRectangularArray;
  using IWedgeDualRectangularArrayPtr = std::shared_ptr<IWedgeDualRectangularArray>;
  using IWedgeDualRectangularArrayConstPtr = std::shared_ptr<const IWedgeDualRectangularArray>;

  class IWedgeImmersion;
  using IWedgeImmersionPtr = std::shared_ptr<IWedgeImmersion>;
  using IWedgeImmersionConstPtr = std::shared_ptr<const IWedgeImmersion>;

  class IWedgeRectangularArray;
  using IWedgeRectangularArrayPtr = std::shared_ptr<IWedgeRectangularArray>;
  using IWedgeRectangularArrayConstPtr = std::shared_ptr<const IWedgeRectangularArray>;

  class IEquipmentFactory
  {
  public:

    virtual ~IEquipmentFactory() = default;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates wedge immersion
    ///
    /// @param materialName      Name of the material.
    /// @param materialVelocity The material velocity.
    /// @param angle            The angle.
    /// @param roof              The roof.
    /// @param height            The height.
    ///
    /// @returns The new wedge immersion.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual IWedgeImmersionPtr CreateWedgeImmersion(const std::wstring& materialName, double materialVelocity, double angle, double roof, double height) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a wedge conventional.
    ///
    /// @param model  The model.
    /// @param length The length.
    /// @param width  The width.
    /// @param height The height.
    /// @param angle  The angle.
    ///
    /// @return
    /// The new wedge conventional.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IWedgeConventionalPtr CreateWedgeConventional(const std::wstring& model, double length, double width, double height, double angle) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a wedge rectangular array.
    ///
    /// @param model  The model.
    /// @param length The length.
    /// @param width  The width.
    /// @param height The height.
    /// @param angle  The angle.
    ///
    /// @return
    /// The new wedge rectangular array.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IWedgeRectangularArrayPtr CreateWedgeRectangularArray(const std::wstring& model, double length, double width, double height, double angle) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a wedge dual rectangular array.
    ///
    /// @param model  The model.
    /// @param length The length.
    /// @param width  The width.
    /// @param height The height.
    /// @param angle  The angle.
    ///
    /// @return
    /// The new wedge dual rectangular array.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IWedgeDualRectangularArrayPtr CreateWedgeDualRectangularArray(const std::wstring& model, double length, double width, double height, double angle) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a conventional transducer rectangular.
    ///
    /// @param model     The model.
    /// @param frequency The frequency.
    /// @param length    The length.
    /// @param width     The width.
    ///
    /// @return
    /// The new conventional transducer rectangular.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransducerConventionalRectangularPtr CreateTransducerConventionalRectangular(const std::wstring& model, double frequency, double length, double width) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a conventional transducer round.
    ///
    /// @param model     The model.
    /// @param frequency The frequency.
    /// @param diameter  The diameter.
    ///
    /// @return
    /// The new conventional transducer round.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransducerConventionalRoundPtr CreateTransducerConventionalRound(const std::wstring& model, double frequency, double diameter) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a transducer rectangular array.
    ///
    /// @param model         The model.
    /// @param frequency       The frequency.
    /// @param nbElementsPrimary   The nb elements primary.
    /// @param nbElementsSecondary The nb elements secondary.
    /// @param elementLength     Length of the element.
    /// @param elementWidth     Width of the element.
    ///
    /// @return
    /// The new transducer rectangular array.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITransducerRectangularArrayPtr CreateTransducerRectangularArray(
      const std::wstring& model, double frequency, int nbElementsPrimary, int nbElementsSecondary, double elementLength, double elementWidth) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates conventional round
    ///
    /// @param name            The name.
    /// @param transducerRound The transducer round.
    /// @param wedgeImmersion  The wedge immersion.
    ///
    /// @returns The new conventional round.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeConventionalPtr CreateConventionalRound(const std::wstring& name, ITransducerConventionalRoundPtr transducerRound, IWedgeImmersionPtr wedgeImmersion) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a conventional round.
    ///
    /// @param name         The name.
    /// @param transducerRound   The transducer round.
    /// @param wedgeConventional The wedge conventional.
    ///
    /// @return
    /// The new conventional round.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeConventionalPtr CreateConventionalRound(const std::wstring& name, ITransducerConventionalRoundPtr transducerRound, IWedgeConventionalPtr wedgeConventional) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a conventional round.
    ///
    /// @param name       The name.
    /// @param transducerRound The transducer round.
    ///
    /// @return
    /// The new conventional round.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeConventionalPtr CreateConventionalRound(const std::wstring& name, ITransducerConventionalRoundPtr transducerRound) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates conventional rectangular
    ///
    /// @param name                  The name.
    /// @param transducerRectangular The transducer rectangular.
    /// @param wedgeImmersion        The wedge immersion.
    ///
    /// @returns The new conventional rectangular.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeConventionalPtr CreateConventionalRectangular(const std::wstring& name, ITransducerConventionalRectangularPtr transducerRectangular, IWedgeImmersionPtr wedgeImmersion) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a conventional rectangular.
    ///
    /// @param name           The name.
    /// @param transducerRectangular The transducer rectangular.
    /// @param wedgeConventional     The wedge conventional.
    ///
    /// @return
    /// The new conventional rectangular.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeConventionalPtr CreateConventionalRectangular(const std::wstring& name, ITransducerConventionalRectangularPtr transducerRectangular, IWedgeConventionalPtr wedgeConventional) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a conventional rectangular.
    ///
    /// @param name           The name.
    /// @param transducerRectangular The transducer rectangular.
    ///
    /// @return
    /// The new conventional rectangular.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeConventionalPtr CreateConventionalRectangular(const std::wstring& name, ITransducerConventionalRectangularPtr transducerRectangular) = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates rectangular array
    ///
    /// @param name            The name.
    /// @param transducerArray Array of transducers.
    /// @param wedgeImmersion  The wedge immersion.
    ///
    /// @returns The new rectangular array.
    /////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeRectangularArrayPtr CreateRectangularArray(const std::wstring& name, ITransducerRectangularArrayPtr transducerArray, IWedgeImmersionPtr wedgeImmersion) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a rectangular array.
    ///
    /// @param name       The name.
    /// @param transducerArray Array of transducers.
    /// @param wedgeArray     Array of wedges.
    ///
    /// @return
    /// The new rectangular array.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeRectangularArrayPtr CreateRectangularArray(const std::wstring& name, ITransducerRectangularArrayPtr transducerArray, IWedgeRectangularArrayPtr wedgeArray) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a rectangular array.
    ///
    /// @param name       The name.
    /// @param transducerArray Array of transducers.
    ///
    /// @return
    /// The new rectangular array.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeRectangularArrayPtr CreateRectangularArray(const std::wstring& name, ITransducerRectangularArrayPtr transducerArray) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a dual rectangular array.
    ///
    /// @param name         The name.
    /// @param pulsingTransducer   The pulsing transducer.
    /// @param receivingTransducer The receiving transducer.
    /// @param wedgeDualArray     Array of wedge duals.
    ///
    /// @return
    /// The new dual rectangular array.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeDualRectangularArrayPtr CreateDualRectangularArray(const std::wstring& name,
      ITransducerRectangularArrayPtr pulsingTransducer, ITransducerRectangularArrayPtr receivingTransducer, IWedgeDualRectangularArrayPtr wedgeDualArray) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates a dual rectangular array.
    ///
    /// @param name         The name.
    /// @param pulsingTransducer   The pulsing transducer.
    /// @param receivingTransducer The receiving transducer.
    ///
    /// @return
    /// The new dual rectangular array.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IProbeDualRectangularArrayPtr CreateDualRectangularArray(const std::wstring& name,
      ITransducerRectangularArrayPtr pulsingTransducer, ITransducerRectangularArrayPtr receivingTransducer) = 0;

    virtual IAcquisitionUnitPtr AddAcquisitionUnit(const std::wstring& name,
      Instrumentation::IDeviceInfo::Platform platform, Instrumentation::IDeviceInfo::Model model) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creates an encoder
    ///
    /// @param  name                The name of this encoder.
    /// @param  distanceResolution  The distance resolution in step/mm.
    ///
    /// @returns  The new encoder.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IEncoderSensorPtr CreateEncoder(const std::wstring& name, double distanceResolution) = 0;

    //TODO:
    //IMultiElementSingleConnectorPtr CreateMultiElementProbeConnector(std::size_t) = 0;
    //IMultiElementDualConnectorPtr CreateMultiElementDualProbeConnector(IMultiElementSingleConnectorPtr, IMultiElementSingleConnectorPtr) = 0;
  };

  using IEquipmentFactoryPtr = std::shared_ptr<IEquipmentFactory>;
  using IEquipmentFactoryConstPtr = std::shared_ptr<const IEquipmentFactory>;
  }
}
