
#pragma once

/*
 
    Class Diagram & Design Patterns:
    --------------------------------
    Root
    ├── statuslogs (Singleton pattern)
    │   ├── + getinstance()
    │   └── + addLogMessage()
    │
    ├── nFactoryFrame (Factory pattern)
    │   ├── + createLogFrame()
    │   └── + createGraphicsFrame()
    │
    ├── nFrame (Composite pattern)
    │   ├── + createFrame()
    │   └── Inherited by:
    │       ├── nLogFrame
    │       │   ├── + createFrame()
    │       │   └── + update()
    │       └── nGraphicsFrame
    │           ├── + createFrame()
    │           └── + update()
    │
    ├── UIFrame (Singleton pattern)
    │   ├── + getInstance()
    │   └── + mainloop()
    │
    ├── nObserver (Observer pattern)
    │   ├── + update()
    │   └── Implemented by:
    │       ├── nLogFrame
    │       └── nGraphicsFrame
    │
    └── ObserverMgr (Observer pattern)
        ├── nSubject
        │   ├── + addObserver(nObserver)
        │   └── + notify()
        └── Notifies Observers:
            ├── nLogFrame
            └── nGraphicsFrame

    Sequence Diagram:
    -----------------
    Root
    ├── Program Start
    │   ├── MainLoop calls UIFrame
    │   │   └── UIFrame creates frames via nFactoryFrame
    │   │       ├── createLogFrame()
    │   │       │   └── nLogFrame created
    │   │       └── createGraphicsFrame()
    │   │           └── nGraphicsFrame created
    │   └── UIFrame calls notify() on nSubject
    │
    ├── Notification Phase
    │   ├── nSubject notifies:
    │   │   ├── nLogFrame
    │   │   │   └── nLogFrame calls update()
    │   │   └── nGraphicsFrame
    │   │       └── nGraphicsFrame calls update()
    │
    └── Observers Update
        ├── nLogFrame updates display
        └── nGraphicsFrame updates display


*/

/*
+----------------------------------+       +----------------------------------+
|           mainwindow (UIFrame)   |       |        ObserverMgr               |
|----------------------------------|       |----------------------------------|
| +getInstance(): UIFrame          |<----->| +notify(x,y,z): void             |
| +mainloop(): int                 |       | +addObserver(nObserver): void    |
| +refreshxyz(x,y,z): void         |       +----------------------------------+
| +logical(nFrame*): void          |
| +receiveData(): void             |
| +sendDataToProcessor(): void     |
+----------------------------------+
        |           ^                +----------------------------------+
        |           |                |          AscanProcessor          |
        |           |                |----------------------------------|
        |<----------+--------------->| +analyze(nFrame*): bool          |
        v                            | +processData(): void             |
+-------------------+----+           +----------------------------------+
|       FactoryMgr        |
|-------------------------|              +----------------------------+
| +LogFrame()           |       nFrame               |
| +GraphicsFrame()     |----------------------------|
| +BscanFrame()        | +CreateColorPalette(): void|
+-----------------------------------------| +setScandat(): void        |
                                          +----------------------------+
                ^       ^           ^               ^       ^
                |       |           |               |       |
+---------------+--+ +--+-----------+-------+   +----+-------+-----+
|   nLogFrame       | |  CscanFrame (nFrame) |   | BscanFrame (nFrame)|
|-------------------| |----------------------|   |--------------------|
| +createFrame(): QWidget| +createFrame(): QWidget| +createFrame(): QWidget|
| +update(): void        | +update(): void        | +update(): void        |
+----------------------- +----------------------- +----------------------+





    Class Diagram & Design Patterns:
    --------------------------------
    +-----------------+              +--------------------------+              +--------------------+
    |   statuslogs    |              |       nFactoryFrame      |              |       nFrame       |
    |-----------------|              |------------------------- |              |--------------------|
    | + getinstance() |<-- Singleton | + createLogFrame()       |<-- Factory   | + createFrame()    |<-- Composite
    | + addLogMessage |              | + createGraphicsFrame()  |              |                    |
    +-----------------+              +--------------------------+              +--------------------+
            ^                                                                 /                    \
            |                                                                /                      \
            |                                                               /                        \
    +----------------------+                                    +--------------------+         +----------------------+
    |      UIFrame         |                                    |    nLogFrame       |         |    nGraphicsFrame    |
    |----------------------|                                    |--------------------|         |----------------------|
    | + getInstance()      |<-- Singleton                       | + createFrame()    |         | + createFrame()      |
    | + mainloop()         |                                    | + update()         |         | + update()           |
    +----------------------+                                    +--------------------+         +----------------------+
                                                                        |                                 |
                                                                Implements Observer               Implements Observer
                                                                        |
                                                                +---------------------+
                                                                |    nObserver        |
                                                                |---------------------|
                                                                | + update()          |
                                                                +---------------------+

    ObserverMgr:
    +-----------------------------------+
    |      nSubject (ObserverMgr)       |
    |-----------------------------------|
    | + addObserver(nObserver)          |
    | + notify()                        |
    +-----------------------------------+
                            |
                      Notifies Observers
                            |
             +--------------------------+
             |                          |
      +--------------------+    +----------------------+
      |    nLogFrame       |    |    nGraphicsFrame    |
      +--------------------+    +----------------------+

    Sequence Diagram:
    -----------------
    1. Program Start:
    +--------------+      +----------------+      +----------------------+      +----------------------+
    |   MainLoop   |      |     UIFrame    |      |       nFactoryFrame  |      |      nLogFrame       |
    +--------------+      +----------------+      +----------------------+      +----------------------+
           |                    |                         |                            |
           |                    |    createLogFrame()     |                            |
           |                    +------------------------>+                            |
           |                    |                         +--------------------------->+
           |                    |                         |                            |
           |                    |    createGraphicsFrame()|                            |
           |                    +------------------------>+                            |
           |                    |                         +--------------------------->+
           |                    |                         |                            |
           |------------------- notify() --------------------------------------------------->
    2. nLogFrame and nGraphicsFrame are updated:
    +--------------------+          +----------------------+           +----------------------+
    |    nSubject        |          |      nLogFrame       |           |   nGraphicsFrame      |
    +--------------------+          +----------------------+           +----------------------+
           |                           |                               |
           |------ notifyObservers() --+                               |
           |                           |------- update() --------------+
           |                           |                               |-------- update() -----+


*/

/*

State Diagram:

[Initial State] ----------------------> [Waiting for Update]
                                         |  (notify() called)
                                         v
                              [Receiving Data from nSubject]
                                         |
                                         v
                            [Processing Data in nLogFrame]
                                         |
                                         v
                                [Update nGraphicsFrame]
                                         |
                                         v
                       [Rendering Graphics in nGraphicsFrame]
                                         |
                                         v
                         [Return to Waiting for Update State]
                         (notify() called or data change detected)
---------------------------------------------------------
Error State:
   - If data processing or rendering fails, transition to
     [Error State] and log the issue using StatusLogs.


*/

/*
Performance Considerations:
---------------------------------------------------------
1. CPU-GPU Workload Division:
   - CUDA for compute-heavy tasks
   - Vulkan for render-heavy tasks (e.g., 3D rendering).
   - Ensure asynchronous data transfer between CPU and GPU.

2. CUDA-Vulkan Interop:
   - Use CUDA-Vulkan Interop to share resources directly (zero-copy).
   - Avoid data transfer overhead with mapped memory.

3. GPU Memory Management:
   - Use Unified Memory to share data between CPU and GPU.
   - Implement memory pooling in Vulkan to reduce memory allocation overhead.

4. Render Pipeline Optimization:
   - Prebuild Vulkan pipeline state objects (PSO) for faster execution.
   - Reuse command buffers and descriptor sets to minimize setup cost.

5. Scalability:
   - Use Dynamic Level of Detail (LOD) to reduce polygon count.
   - Implement Frustum Culling to avoid rendering unnecessary objects.
   - Take advantage of Vulkan’s multithreading for parallel rendering.

6. Real-time Rendering:
   - Employ Vulkan async compute to handle continuous data streams.
   - Reduce latency with low-latency rendering (reduce buffer and queue depth).

7. Error Handling:
   - Use cudaGetLastError() for error checks in CUDA.
   - Enable Vulkan Validation Layers to catch runtime issues.

*/