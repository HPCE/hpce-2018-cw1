Windows as a platform
===================

If you are using Windows, you have a few choices:

1. Windows 10 only: Using the [Windows Subsystem for Linux](https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux).
   This will work well for the multi-core (TBB) parts, though compatability with OpenCL is less likely to work (WSL is very new).
   [Installation instructions](https://msdn.microsoft.com/en-gb/commandline/wsl/install_guide) are available from Microsoft.

2. Install a [Linux virtual machine](readme_linux.md). This actually works
    very well, and is well worth considering.

3. Use [MinGW](http://mingw.org/). This provides access to both TBB and OpenCL.
   I prefer not to give people canned versions, but for those who don't have
   immedate access to VMWare, I prepared a [canned version](https://imperialcollegelondon.app.box.com/v/HPCE-2016-mingw64-tbb).
   Unzip it somewhere, then you can start it up immediately.

Note that Cygwin is not really a viable option, as TBB does not work (though OpenCL will).

Compiling with Visual Studio
----------------------------

It is possible to do your development work in Visual Studio, as the
code we will use is quite portable: [TBB exists for windows](https://www.threadingbuildingblocks.org/download#stable-releases),
and OpenCL is also available. I'm not going to provide too much support
here, as I'd prefer to get people away from the crutch of IDEs,
at least for now.
