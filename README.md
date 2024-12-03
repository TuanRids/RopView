# 3D PAUT Visualization and Real-time Robotics Integration

## Overview
This project focuses on real-time 3D visualization of Phased Array Ultrasonic Testing (PAUT) data and integrates it with robotic control for industrial applications. It leverages Vulkan for high-performance rendering and includes plans for AI-based optimization in the future.

## Key Features
### 1. Phased Array Ultrasound in Realtime
- Real-time control and processing of OmniScan PAUT data.
- Full rendering and handling of A, B, S, and C-scan data.
- 3D data visualization using Vulkan.
- **Results**:
  - Achieved ~1.8 ms per frame (~500 FPS), bottleneck at PAUT device.
  - Resolutions: Cview 920x373, Bview 920x372, Sview 722x373, Aview 722x372 (total ~2 million pixels).
  - Total frametime: ~2 ms (~500 FPS).
  - Highly efficient for real-time industrial use cases.
- Real-time control and processing of OmniScan PAUT data.
- Full rendering and handling of A, B, S, and C-scan data.
- 3D data visualization using Vulkan.

### 2. Robot Control in Realtime
- Real-time robotic arm control with IPC and C++.
- Vision-based detection for autonomous PAUT scanning.

### 3. AI Application (Planned)
- AI integration to enhance FEM analysis and optimize performance.
- Plans to use GPU-based rendering and OpenCV for AI detection at high frame rates.

### 4. FEM Integration
- Finite Element Method (FEM) applied to processed PAUT data.
- CUDA acceleration for FEM calculations.
- Integrated into Vulkan viewport for simulation.

## Technologies Used
- **PAUT**: Real-time Phased Array Ultrasound processing (C++)
- **Robotics**: Real-time Yaskawa robot arm control (C++)
- **Vulkan**: High-performance 3D rendering (C++)
- **IPC**: Inter-process communication for real-time systems (C++)
- **FEM**: Finite Element Method simulation (C++)
- **CUDA**: Accelerated FEM computation (C++)
- **AI (Planned)**: AI optimization and real-time detection.

## Results (Ongoing)
- **Read PAUT Data**: Achieved ~1.8 ms per frame (~500 FPS), bottleneck at PAUT device.
- **Render and Process Images**:
  - Resolutions: Cview 920x373, Bview 920x372, Sview 722x373, Aview 722x372.
  - Total frametime: ~2 ms (~500 FPS).
- **Performance**: Highly efficient for real-time industrial use cases.

This system combines real-time robotics, advanced 3D rendering, and scalable data processing to support high-performance industrial applications.
