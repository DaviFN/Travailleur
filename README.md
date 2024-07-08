Source dump of a desktop automation framework I coded a while ago.

Travailleur is a desktop automation framework that provides functionalities such as image pattern recognition, OCR (optical character recognition), image preprocessing with various abstractions from OpenCV library, video input from media source (e.g. Webcam), video input from the desktop itself, mouse outputs and a few keyboard outputs. It is intended to be easily configurable via graphical user interface, allowing to quickly automate a sequence of actions in the desktop, possibly based on visual input from the screen or any other video media source.
Its name comes from the French "worker", which is a somewhat good description of the intention of the project (automate a task on the desktop, and then leave it to be done by the "worker").

Technologies used: C++, CMake, OpenGL, OpenCV, Tesseract, Dear ImGui immediate GUI library
