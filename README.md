# ImWindow

[![Build status](https://ci.appveyor.com/api/projects/status/k72i9fqi4px68oxl?svg=true)](https://ci.appveyor.com/project/thennequin/imwindow)

Window and GUI system based on ImGui from OCornut.

Include docking/floating window, multi window and multi render support.

![Screenshot1](https://raw.githubusercontent.com/wiki/thennequin/ImWindow/images/Screen1.png)

![Docking](https://raw.githubusercontent.com/wiki/thennequin/ImWindow/images/Docking.gif)

# Platform
Actually work on Windows with different backend
  * DX11
  * OpenGL
  * OpenGL with GLFW
  * BGFX
  
Linux support is in progress.

For Mac it's not decided yet.

# License
ImWindow is licensed under the MIT License, see LICENSE for more information.

# How to build

- clone the project together with the `imgui` submodule:

`git clone --recursive https://github.com/thennequin/ImWindow.git`

- run on of the `.bat` under `/scripts`

- if you don't have it yet, install the [DirectX End-User Runtime](https://www.microsoft.com/en-us/download/details.aspx?displaylang=en&id=35)

- for each of the two project, update your `Windows SDK Version` to match your corresponding version
