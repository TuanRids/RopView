
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