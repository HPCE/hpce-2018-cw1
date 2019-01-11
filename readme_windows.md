Windows as a platform
===================

If you are using Windows, you have a few choices:

1. Use [MinGW](http://mingw.org/). This provides access to both TBB and OpenCL.
   I prefer not to give people canned versions of environments, but traditionally
   setting up things has blocked people more than it should, so: [canned version](https://imperialcollegelondon.app.box.com/v/HPCE-2018-mingw64-tbb-opencl). Note that this file is ~700MB, so I strongly recommend you download it in college, and start the download
   before you want to use it.
   Unzip it somewhere, then you can start it up immediately. You'll need around 2GB wherever you install it, so a 4GB
   USB drive works quite well, and makes it portable.

2. Windows 10 only: Using the [Windows Subsystem for Linux](https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux).
   This will work well for the multi-core (TBB) parts, though as of early 2019 there is
   still no support for OpenCL in WSL.
   [Installation instructions](https://msdn.microsoft.com/en-gb/commandline/wsl/install_guide) are available from Microsoft.

3. Install a [Linux virtual machine](readme_linux.md). This actually works
    very well for TBB (though less well for OpenCL), and is well worth considering as a learning experience...

Note that Cygwin is not really a viable option, as TBB does not work (though OpenCL will...).

Compiling with Visual Studio
----------------------------

It is possible to do your development work in Visual Studio, as the
code we will use is quite portable: [TBB exists for windows](https://www.threadingbuildingblocks.org/download#stable-releases),
and OpenCL is also available. I'm not going to provide too much support
here, as I'd prefer to get people away from the crutch of IDEs,
at least for now.
