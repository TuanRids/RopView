# 3D PAUT Visualization and Real-time Robotics Integration

## Overview
This project provides real-time 3D visualization of **Phased Array Ultrasonic Testing (PAUT)** data using **Vulkan**, integrated with **Finite Element Method (FEM)** analysis. It also incorporates real-time control of a **robot** to scan objects using **OmniScan**, and includes future plans for **AI**-based optimization in FEM processing.

## Key Features

### 1. Phased Array Ultrasound in Realtime
- Real-time controlling and processing of the OmniScan data.
- Calculate, control, and render PAUT data.
- 3D data processing from **Ultrasound results**.
- Uses **Vulkan** to create a real-time 3D environment.
- Full handling of **A, B, S, and C-scan** data.

### 2. Robot Control in Realtime
- Real-time robot control and visualization using **IPC** or **pure C++**.
- **Vision detection** integrated to assist in autonomous PAUT scanning.
  
### 3. AI Application (Upcoming)
- AI integration planned to enhance FEM analysis and optimize performance:
- **Method Summary:**
- Use OpenGL/Vulkan for rendering PAUT data at super high fps (every 3-6 ms).
- Pass the rendered TextureID to OpenCV for AI detection at 60 fps (every 16 ms).
- **Key Steps:**
- Use OpenGL/Vulkan to continuously load PAUT data onto GPU and render it in real-time.
- Extract TextureID from GPU to pass it to OpenCV for AI processing on a separate layer.
- **Pros and Cons:**
- Pros: Efficient resource allocation, high rendering speed (100-150 fps), separate layers for rendering and AI, ensuring optimal performance for each.
- Cons: Requires careful GPU resource management, synchronization between layers may add complexity.

### 4. FEM Integration
- **Finite Element Method (FEM)** applied to the processed PAUT data.
- Use CUDA for calculating.
- Integrated into the Vulkan 3D viewport for accurate simulation.

  
## Main Technologies Used
- **PAUT**: Phased Array Ultrasound (C++)
- **ROBOT**: Robot Arm Yaskawa (C++)
- **Vulkan**: Real-time 3D rendering and viewport creation (C++)
- **C++**: Core development language for real-time control and data processing (C++)
- **IPC**: For real-time communication between systems (C++)
- **FEM**: Applied for accurate simulation based on PAUT data (C++)
- **AI (Planned)**: Future integration to optimize FEM processing (C++)
- **CUDA**: For applying FEM Integration (C++)
- OThers...

This project combines cutting-edge techniques in **real-time 3D graphics**, **robotics**, and **data processing**, with plans for **AI**-based analysis enhancements. The system is designed for scalability and advanced use cases, such as real-time industrial applications.
