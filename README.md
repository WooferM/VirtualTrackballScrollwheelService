Virtual Trackball Scrollwheel Service
===============
**Allows scrolling with a trackball or other pointing device without a scroll wheel by using a low level mouse hook.**

Changes the behaviour of any X-Button (typically buttons 3 and 4) and when both left and right mouse buttons are held to scroll. i.e. vertical mouse wheel events, when an X-Button or both left and right buttons are pressed and the trackball is moved

Based on https://github.com/Seelge/TrackballScroll

###### Requirements
- A trackball or mouse
- A Microsoft Windows operating system

This software has been tested with a *Logitech Marge Mouse* (tm) (2 button) and *Logitech Trackman Marble* (tm) in *Microsoft Windows 7*

###### Download the latest release
https://github.com/WooferM/VirtualTrackballScrollwheelService/releases/latest

###### Run the program
- Execute `VirtualTrackballScrollwheelService.exe`, no installation is necessary.
- Add to the Startup folder in your start menu to run automatically on boot.
- When using this program with a driver software that allows customization of the button behavior, make sure to set the X-Buttons to default behavior. E.g. with a *Logitech Trackman Marble*, make sure to set the buttons 3 and 4 to `default` button behaviour or `Generic Button` and not `back`/`Universal Scroll`.
- The lines scrolled per wheel event are determined by the *Microsoft Windows* mouse wheel settings.

###### Compile the source code
- Clone the repository from the github page or download the latest source code archive
- Open the solution with *Microsoft Visual Studio 2010*. If you are using a different version, create a new solution and add the missing files.
- Change the build type to `Release` and `x64`. The program also works in both 64 and 32 bit programs.
- Build the solution
- `VirtualTrackballScrollwheelService.exe` will be created in the `\Release` folder 