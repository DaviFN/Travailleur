# Travailleur
**Desktop Automation Framework with Multi-Source Video Stream Input and Computer Vision Functionalities**

---

# Overview 
Travailleur is a C++ automation framework that combines computer vision techniques (image pattern recognition, OCR) with desktop automation functionalities that offers a robust way to create desktop automation workflows. It supports multiple video sources including live screens, webcams, recordings, and pre-existing media, and allows users to create custom apps that have access to an easy-to-use API.

**Disclaimer**: This project was originally developed for personal use and is by no means a finished product. It is provided **"as is"** without any kind of warranty. 

---

## Video Demonstrations

*The following playlist contains a series of demonstration videos showcasing selected functionalities of the framework that should give you a good idea of how Travailleur works and what it has to offer. They can help you understand how the components work together and inspire ideas for your own projects.*

[Watch the Travailleur Video Demonstrations Playlist on YouTube](https://www.youtube.com/playlist?list=PLVME6v4iHn2s7oK3sfgOAWSepkH8Oy-3z)

---

# Trivia
"Travailleur" comes from the French language and means "worker," alluding to the idea of creating an automation akin to a worker.  

---

## Key Features

### Multi-Source Video Input 
*Travailleur allows capturing from a variety of sources to maximize automation flexibility and coverage. The current captured frame is used as input for the visual functionalities, both when an application's components are being configured and when an application is running.*  
- **Desktop Capture**: Desktop as capture input
- **Webcam Streaming**: Webcam device as capture input
- **Media Playback**: Screenshots/videos as capture input, which can be taken/recorded using the framework itself and having as source the current media being captured
- **Frame Control**: A specific frame in a video as capture input

### Visual Processing Functionalities
*These can be used both when the framework is being configured or programatically by a user-defined application.*
- **Pattern Recognition**: Image detection with a custom algorithm
- **OCR Text Reading**: Tesseract-powered text recognition
- **Preprocessing Pipeline**: Image pattern enhancement/optimization 

### Component Configuration
*Components are configurable pieces of information for a given application. Their existence is defined beforehand in the app's code, and they can be configured as desired using the configuration tools provided by the framework to best achieve the application's objective, whatever it may be.*
- **Image Pattern Acquisition**: Define image patterns to be used in the custom image pattern detection algorithm
- **Rectangular Regions**: Define screen areas in which most operations are performed; their coordinates can be absolute, relative to other rectangular regions or relative to a given image pattern detection within the current frame, allowing for a variety of use cases
- **OCR Profiles**: Define a specific text recognition input settings, such as the region in which it will be performed and whether preprocessing steps are to be performed to the image
- **Image Processing Steps**: Enhance OCR's accuracy by easily configuring image processing steps such as scaling, thresholding and denoising, to be performed before OCR takes place
- **Coordinate Input**: Easily define coordinates in the screen, which can then be referenced by the mouse-related functionalities in the API

### Travailleur's applications
*The framework allows developers to create custom applications, called "Travailleur applications". Once an application's components are configured, the application code can use Travailleur's API, which provides basic functionalities needed for a desktop automation application, and can have as video input any video stream source Travailleur provides. Aside from that, of course, the app can also use any functionality/library the C++ language provides.*
- **Custom User Interface**: The app can have its own user interface by using the Dear ImGui immediate-mode graphical user interface, in which Travailleur is built in
- **Image Pattern Detection**: Programatically check whether a given image is within a given search area
- **Text Extraction**: Programatically extract text from a given area
- **Mouse and Keyboard Control**: Programatically control the mouse and/or the keyboard by moving the mouse, issuing mouse clicks and/or key presses

---

### Steps to Create a Travailleur Application  
*Travailleur applications are executed within the framework and serve as the core mechanism for creating desktop automation workflows. These applications leverage the framework's computer vision and input control capabilities, with default examples provided as starter templates. This section outlines the fundamental steps to build a Travailleur application.*


1. **Inherit From TravailleurApp**  
   *Create a new class inheriting from the TravailleurApp class*  
2. **Implement Core Methods**  
   *These methods **must** be implemented in order for your application to work*  
   - `getName()` → *Your app's name*  
   - `setup()` → *Where you define the app's components*    
     - **Use the following register methods to define your app's components**:  
       - `registerPointInput(<String>) ` → Define a coordinate input
       - `registerRectangularRegion(<String>)`→ Define a rectangular region
       - `registerImagePattern(<String>)` → Define an image pattern
       - `registerImagePreprocessing(<String>)` → Define an image preprocessing profile
       - `registerImageOCR(<String>)` → Define an image OCR profile
       - `registerImageResource(<String>)` → Define an image resource, that can be programatically shown on the screen at any time  

    **Important: When using the API, the strings used to reference the components need to exactly match the ones defined here**
   - `processFrame()` → *Here goes your frame processing logic, where you can access Travailleur's API to code your automation*  
   - `showPreRunGui()` → *Pre-launch configuration UI; this can be useful, for example, for exposing configuration options to users*  
   - `showRunningGui()` → *This is where you code the runtime interface for your app*  
3. **Configure Components**  
   *From the previous step, your app is already created, and now you can launch Travailleur to configure the components you previously defined*
---

## API Usage Guide

*Once you have a Travailleur application, use the API to perform operations and obtain information based on the configuration of the components you defined in your app*

- `hasImagePattern(<ImagePatternName>, <RectangularRegionName>)` → *Use this method to check whether the image pattern referenced by the string in the first parameter is currently within the rectangular region referenced by the string in the second parameter*  
- `performImageOcr(<ImageOcrName>, <RectangularRegionName>)` → *Use this method to extract text from the rectangular regions referenced by the string in the second parameter using the OCR profile referenced by the string in the first parameter*  
- `moveMouse(<Absolute Coordinate>/<Coordinate Input>)` → *Use this method to move the mouse either to an absolute coordinate or to a coordinate input referenced by a string*  
- `pressLeftMouseButton/pressRightMouseButton(<Press Duration>)` → *Use this method to press a mouse button for a specified duration of time*  
- `pressKey(<Key>, <Press Duration>)` → *Use this method to press a keyboard key for a specified duration of time*  
- `showImageResource(<ImageResourceName>)` → *Use this method to show a given image resource in Travailleur's capture stream area*  


**Pro tip: If you need to perform faster image pattern checking and/or OCR reading within a frame, you can schedule them to run asynchronously using pushAsyncHasImagePattern and/or pushAsyncPerformImageOcr and then run them all in parallel using runAsyncReadings**

---

## Getting Started (Building and Running)

*Follow the following steps to build and run the Travailleur framework on your machine:*

1. **Assure Required Software Are Installed**  
   *Make sure you have installed the following software components on your machine:*
   - Visual Studio (on Windows)
   - Python
   - CMake
   - Make

2. **Install vcpkg and the required vcpkg packages**
   *Travailleur uses vcpkg, a C++ package manager, to manage some of its requirements; make sure to follow the following steps to install vcpkg in your machine and acquire the required packages:*
   1. Navigate to a folder where vcpkg is to be installed; as a suggestion, "C:\devtools"
   2. Clone the vcpkg repository on GitHub and cd onto it:  
      ```bash
      git clone https://github.com/microsoft/vcpkg.git && cd vcpkg
      ```
   3. Run vcpkg's bootstrap batch script, which will download and build the vcpkg executable:
      ```bash
      bootstrap-vcpkg.bat
      ```

      If on Linux:
      ```bash
      ./bootstrap-vcpkg.sh
      ```
   4. Integrate vcpkg with Visual Studio (optional, but recommended):
      ```bash
      vcpkg integrate install
      ```
   5. Add a VCPKG_ROOT environment variable pointing to the vcpkg directory (if the suggestion was followed, it should be "C:\devtools\vcpkg")
   
      If on Linux:
      ```bash
      echo 'export VCPKG_ROOT=$HOME/vcpkg' >> ~/.bashrc
      echo 'export PATH="$VCPKG_ROOT:$PATH"' >> ~/.bashrc
      source ~/.bashrc
      ```
   6. Add an entry to the path environment variable pointing to that same directory to make vcpkg's executable accessible through the command line (restart command line so that this change takes place)   
   > *At this point, vcpkg is installed; now, install the dependencies:*
   7. Install the required packages for the build, which are provided by vcpkg (this might take a while): 
      ```bash
      vcpkg install opencv:x64-windows tesseract:x64-windows gtest:x64-windows sdl3:x64-windows imgui[sdl3-binding,opengl3-binding]:x64-windows
      ```

      If on Linux:
      ```bash
      vcpkg install opencv:x64-linux tesseract:x64-linux gtest:x64-linux sdl3:x64-linux imgui[sdl3-binding,opengl3-binding]:x64-linux
      ```
3. **Setup Tesseract Data** 
   *Tesseract works better if you configure it with adequate language data; we will use the language data called "tessdata_best", but there are others available in [this webpage](https://tesseract-ocr.github.io/tessdoc/Data-Files.html)*
   1. Clone the language data repository somewhere; we suggest "C:\devtools", or the home folder on Linux: 
      ```bash
      cd C:\devtools && git clone https://github.com/tesseract-ocr/tessdata_best
      ```

      If on Linux:
      ```bash
      cd ~ && git clone https://github.com/tesseract-ocr/tessdata_best
      ```
   2. Add a TESSDATA_PREFIX environment variable pointing to that exact directory (e.g. "C:\devtools\tessdata_best")
   
   If on Linux:
   ```bash
   echo 'export TESSDATA_PREFIX="$HOME/tessdata_best"' >> ~/.bashrc
   source ~/.bashrc
   ```
4. **Clone and Build Travailleur** 
   *At this point, we have everything we need to clone and build Travailleur*
   1. Clone Travailleur's repository somewhere and cd onto it:
      ```bash
      git clone https://github.com/DaviFN/Travailleur && cd Travailleur
      ```
   2. Generate the Visual Studio solution by issuing the following command:
      ```bash
      make
      ```
   3. Open the solution with the following command:
      ```bash
      make open
      ```
   4. Open the solution explorer window, if not open already (CTRL + ALT + L hotkey)
   5. Right-click the "INSTALL" project and select "build"; wait until the build process completes
   
> If on Linux, issue "make" on the build folder (buildfiles/debug or buildfiles/relwithdebinfo)
5. **Run the Framework**   
   *Once the solution is built, you can build an app and run it. If you don't have your own yet, you can use the provided DummyProject:*
   1. Set the example project named DummyProject to be the startup one by right-clicking it and selecting "set as startup project"
   2. Launch the app through the Local Windows Debugger (or directly via the generated executable file)


---

## Todos

- [ ] Improve the user interface experience, which is very cumbersome at the moment
- [ ] Reimplement the image pattern detection algorithm leveraging the GPU by using compute shaders, which will drastically improve its performance
- [ ] Make coordinate inputs more flexible; currently they can only be absolute
- [X] Make the framework cross-platform; though Travailleur is currently Windows-specific, most of its code is platform-agnostic, so this should not be so hard to do

---

## Contributing

Contributions are welcome! If you have ideas for new features or improvements, or if you encounter any bugs, feel free to open an issue or submit a pull request.

---

## License

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL%20v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)

This project is licensed under the [GNU Affero General Public License v3.0](https://www.gnu.org/licenses/agpl-3.0).

Third-party libraries use their own licenses:  
- **[OpenCV]** (Apache 2.0 License): <https://opencv.org/license/>  
- **[Tesseract OCR]** (Apache 2.0 License): <https://github.com/tesseract-ocr/tesseract>  
- **[Dear ImGui]** (MIT License): <https://github.com/ocornut/imgui/blob/master/LICENSE.txt>

[OpenCV]: https://opencv.org/license/  
[Tesseract OCR]: https://github.com/tesseract-ocr/tesseract  
[Dear ImGui]: https://github.com/ocornut/imgui/blob/master/LICENSE.txt