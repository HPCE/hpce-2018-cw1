Linux as a platform
===================

Linux generally works well as a platform for this course,
as all the tools are supported natively, and all the libraries
are quite easy to install.

You don't have to be running Linux natively in order to use
it for this course - it works quite well in a virtual machine,
and you get almost full performance from multiple cores.
Support for GPUs from virtual machines is still poor/non-existent,
but it is possible to install a software OpenCL for development purposes,
then do the actual evaluation on a remote linux instance.

In terms of _which_ Linux to choose, arguably the two main
alternatives are Ubuntu or Debian. Ubuntu is probably the
better choice if you want a full install with a GUI, while
Debian is a bit more stable. For our purposes Ubuntu is
a good choice.

Running in AWS
--------------

In the next coursework we will be using Amazon AWS, and this
is the intended assessment environment, so you may wish to
look ahead to the notes in the next coursework and purely
use AWS. However, be aware that time in AWS costs money,
so you should use a cheap instance for development (e.g.
something that only costs 10p an hour). Generally speaking it is
better to use a local machine for development, and only move to
the target environment for testing and performance runs.

Using a virtual machine
----------------------------

I would suggest setting up a virtual machine rather than
a full install of Linux, at least to start with. Later on
if you want to access your GPU from within Linux it will
need to be native, but the idea is that later on we well
use GPUs in AWS, so the performance of _your_ GPU is less
interesting. Dual booting can be useful, but you might find
it gets in the way a bit at first - try not to get blocked
on creating a perfect install of Linux _before_ doing the
first coursework.

You'll need some kind of virtualisation software in order
to run the VM. There are a few options:

- [VirtualBox](https://www.virtualbox.org/wiki/Downloads): works well on both OS X and Windows

- [VMWare](http://www.vmware.com/products/player/playerpro-evaluation.html):
  I only have experience of this under Windows, though don't really use
  it much.

- QEMU: This is more low-level. I wouldn't suggest using this
  (though it is a great piece of software).

Once you've selected or installed the virtualisation software,
you'll need to install the guest OS. This is very similar
to installing an OS on a real machine:

1 - Download a [CD ISO Image](https://www.ubuntu.com/download/desktop).

2 - Create a new virtual machine in your virtualisation software.

3 - Boot the virtual machine with the ISO attached to the DVD drive.

4 - Follow the instructions in the Ubuntu installer.

Once the setup has finished, you should have a disposable Linux instance.

The exact installation steps require some common sense, or possibly
a little research. There are many tutorials on installing Ubuntu
in VirtualBox available with a little googling.

If anyone gets stuck, feel free to post an issue. I'm not too sure
what people will find tricky about the process, so can't really
anticipate the questions.

Installing libraries
--------------------

Given a linux instance, you'll want to install a few tools. If you are
in Ubuntu or Debian, the easiest way to get them is on the command
line using [apt](https://en.wikipedia.org/wiki/Advanced_Packaging_Tool).
To get to a command line, look for an application called `terminal` in
the GUI.

You'll probably want to install a few tools, such as `g++` and `git`. You
can do this with the following command (you may need to type in your super-user
password):

    sudo apt-get install g++ git

Breaking this down:

  - [`sudo`](https://www.xkcd.com/149/) : Run the command as the super-user (equivalent
    to administrator in windows). Installing packages requires system-wide permissions,
    so this is necessary here. Beware of getting _too_ sudo happy, as you should only
    use if for situations where you need to be super-user.

  - `apt-get` : This is one of the components of the `apt` package, responsible for
    fetching and installing packages.

  - `install` : The `apt-get` component has a few functions, and we are selecting the
    `install` function.

  - `g++ git` : These are the two packages we want to install.

If you don't know which package you want, then you can search the database. For
example, we can look for the package containing tbb:

   apt-cache search tbb

In this case we don't need to be super-user, as we are not modifying the system,
so `sudo` is not needed. You should see a list of possible packages. For example,
I get:

    $ apt-cache search tbb
    libthrust-dev - Thrust - Parallel Algorithms Library
    libtbb-dev - parallelism library for C++ - development files
    libtbb-doc - parallelism library for C++ - documentation
    libtbb2 - parallelism library for C++ - runtime files
    libtbb2-dbg - parallelism library for C++ - debugging symbols
    tbb-examples - parallelism library for C++ - example files

We want to develop with tbb, so the package needed is `libtbb-dev`:

    sudo apt install libtbb-dev

Another thing you might want to do is to find the package that provides
a file. For example, we might want to know who provides `parallel_for.h`.
This can be done at the command line, but it is easier to look on the
internet at the [Ubuntu package search](http://packages.ubuntu.com/). If
you search the contents of packages for `parallel_for.h`, it should
tell you that it is in `libtbb-dev`.

Anything else that is missing you can install in the same way (e.g. `make`).

Please file an issue if you need more guidance.

