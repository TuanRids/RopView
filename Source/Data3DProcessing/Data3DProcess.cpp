#include "Data3DProcess.h"

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCellArray.h>
#include <vtkProperty.h>

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
AscanData IData3DProcecss::m_dataset{};
std::vector<Color> CreateColorPalette() {
    int gainFactor = 2;
    // Define a set of key colors that will be used as reference points for the gradient.
    std::vector<Color> colors;
    std::vector<Color> everyColors;

    // Key colors in the gradient, ranging from white to deep red.
    colors.emplace_back(Color{ 255, 255, 255 }); // white
    colors.emplace_back(Color{ 184, 212, 244 }); // light blue
    colors.emplace_back(Color{ 113, 170, 233 }); // blue
    colors.emplace_back(Color{ 62, 105, 190 });  // dark blue
    colors.emplace_back(Color{ 14, 37, 143 });   // dark purple
    colors.emplace_back(Color{ 27, 72, 129 });   // purple
    colors.emplace_back(Color{ 59, 140, 127 });  // green
    colors.emplace_back(Color{ 126, 187, 94 });  // light green
    colors.emplace_back(Color{ 211, 223, 45 });  // yellow
    colors.emplace_back(Color{ 241, 211, 43 });  // light yellow
    colors.emplace_back(Color{ 222, 156, 80 });  // orange
    colors.emplace_back(Color{ 209, 121, 87 });  // light orange
    colors.emplace_back(Color{ 205, 116, 49 });  // dark orange
    colors.emplace_back(Color{ 194, 98, 23 });   // brown
    colors.emplace_back(Color{ 167, 50, 26 });   // dark brown
    colors.emplace_back(Color{ 145, 12, 29 });   // red

    // Number of interpolation points between each pair of key colors.
    size_t interpolationPoints{ 7 };
    double f = 1.0 / interpolationPoints;   // Fractional step size for interpolation.

    // Interpolate between each pair of consecutive key colors.
    for (size_t colorIdx(0); colorIdx < colors.size() - 1; colorIdx++) {
        Color x = colors[colorIdx];         // Start color
        Color y = colors[colorIdx + 1];     // End color

        // Generate intermediate colors between x and y.
        for (size_t j(0); j < interpolationPoints; j++) {
            // Interpolate each RGB component separately.
            uint8_t r = static_cast<uint8_t>(x.R + (y.R - x.R) * (j * f));
            uint8_t g = static_cast<uint8_t>(x.G + (y.G - x.G) * (j * f));
            uint8_t b = static_cast<uint8_t>(x.B + (y.B - x.B) * (j * f));

            // Add the interpolated color to the resulting palette.
            everyColors.emplace_back(Color{ r, g, b });
        }
    }

    // Apply gainFactor to shift colors based on the intensity
    size_t numColors = everyColors.size();
    std::vector<Color> gainAdjustedColors(numColors);

    // Adjust the colors based on the gain factor.
    for (size_t i = 0; i < numColors; ++i) {
        // Calculate the shifted index based on gain.
        int shiftedIndex = static_cast<int>(i * gainFactor);
        if (shiftedIndex >= numColors) {
            shiftedIndex = static_cast<int>(numColors - 1); // Cap at the last color
        }
        gainAdjustedColors[i] = everyColors[shiftedIndex];
    }

    return gainAdjustedColors;

}
void Data3DProcess::processData_volume()
{
    if (m_dataset.Amplitudes.empty()) {
        return;
    }

    auto zsize = m_dataset.AmplitudeAxes[0].Quantity;
    auto ysize = m_dataset.AmplitudeAxes[1].Quantity;
    auto xsize = m_dataset.AmplitudeAxes[2].Quantity;

    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
        imageData->SetDimensions(xsize, ysize, zsize);
        imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

    std::vector<Color> colorpalette = CreateColorPalette();
    for (uint32_t z = 0; z < zsize; ++z) {
        for (uint32_t y = 0; y < ysize; ++y) {
            for (uint32_t x = 0; x < xsize; ++x) {
                uint32_t index = z * (xsize * ysize) + y * xsize + x;
                if (index >= m_dataset.Amplitudes.size()) { return; } // TODO return error
                int16_t amplitude = std::abs(m_dataset.Amplitudes[index]);
                double percentAmp = amplitude / (32768.0 / 100.0);
                unsigned char* pixel = static_cast<unsigned char*>(imageData->GetScalarPointer(x, y, z));
                Color clor = colorpalette[static_cast<int>(percentAmp)];
                pixel[0] = static_cast<unsigned char>((clor.R + clor.G + clor.B) / 3);
            }
        }
    }

    vtkSmartPointer<vtkSmartVolumeMapper> volMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
        volMapper->SetInputData(imageData);

    vtkSmartPointer<vtkVolumeProperty> volProperty = vtkSmartPointer<vtkVolumeProperty>::New();
        volProperty->ShadeOn();
        volProperty->SetInterpolationTypeToLinear();

    vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
        compositeOpacity->AddPoint(0.0, 0.0);
        compositeOpacity->AddPoint(255.0, 1.0);
        volProperty->SetScalarOpacity(compositeOpacity);

    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
        colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
        colorTransferFunction->AddRGBPoint(255.0, 1.0, 1.0, 1.0);
        volProperty->SetColor(colorTransferFunction);

    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
        volume->SetMapper(volMapper);
        volume->SetProperty(volProperty);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
        renderer->AddVolume(volume);
        renderer->SetBackground(0.1, 0.2, 0.4);
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renWinInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        renWinInteractor->SetRenderWindow(renderWindow);


    renderWindow->Render();
    renWinInteractor->Start();
    return;
}
Mesh Data3DProcess::processData()
{
    // processData_volume();
    if (m_dataset.Amplitudes.empty()) {
        return Mesh(); //TODO return error
    }
    Mesh mesh;
    mesh.name = "Paut Object";
    auto zsize = m_dataset.AmplitudeAxes[0].Quantity;
    auto ysize = m_dataset.AmplitudeAxes[1].Quantity;
    auto xsize = m_dataset.AmplitudeAxes[2].Quantity;

    double percentAmplitude;
    // Processing amplitude data to assign colors
    for (uint32_t y = 0; y < ysize; ++y) {
        for (uint32_t x = 0; x < xsize; ++x) {
            for (uint32_t z = 0; z < zsize; ++z) {
                uint32_t index = z * (xsize * ysize) + y * xsize + x;
                if (index >= m_dataset.Amplitudes.size()) { return Mesh(); } // TODO return error
                int32_t samplingAmplitude = std::abs(m_dataset.Amplitudes[index]);
                percentAmplitude = samplingAmplitude / (32768.0 / 100.0);
                if (percentAmplitude > 10) {
                    mesh.vertices.push_back(Vertex(glm::vec3(x *0.01f , y *0.01f, z *0.01f), glm::vec3(0.9f, 0.0f, 0.0f)));
                }
                /*else {
                    mesh.vertices.push_back(Vertex(glm::vec3(x *0.01f , y *0.01f, z *0.01f), glm::vec3(0.5f, 0.5f, 0.5f)));
                }*/
            }
        }
    }
    // add 1 indices 0

    return mesh;
}
