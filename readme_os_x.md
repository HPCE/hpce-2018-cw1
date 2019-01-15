OS X as a Platform
==================

OS X is pretty much unix, as the user facing tools share a lot
with BSD (a variant of unix). However, the command line tools
that come with OS X by default are not really up to scratch,
so you need to install or overwrite some of the tools. You'll
also want to install extra libraries, such as TBB.

Note: while many students have Macs, this isn't something
that is used extensively by the teaching staff, or widely
used for high-performance research. It is a decent platform,
but you may need to play around with it more than in unix,
and may well need to rely on advice from other students
who have Macs for things like installing software.

You have three main choices:

1 - Use the [`homebrew`](http://brew.sh/) package manager to install
    and manage the additional tools.

2 - Use the [`macports`](https://www.macports.org/) package manager
    to install and manage the additional tools.

3 - Run [Linux in a virtual machine](platforms_linux.md). For OS X
    I would suggest one of the above options in preference to a VM.

_Note: Student consensus seems to be that brew is easier than ports, see [issue 17](https://github.com/HPCE/hpce-2017-cw1/issues/17)._

Some people like macports, some like homebrew - personally I've only
used macports, and that was a few yeas ago. There are [instructions for installing macports](https://www.macports.org/install.php)
on the web-site, and the process is pretty reliable.

Once macports is installed, you have a command line package manager
installed. For example, if you do:

    port search tbb

Then it will look for packages containing tbb. In this case the
package is just called `tbb`, so the following will install it:

    sudo port install tbb

The `sudo` command allows it to run as administrator, so that it
can change system-wide settings. You can also install other tools,
for example you probably want `make` and `git`:

    sudo port install make git

The macports package provides a very similar experience to Linux,
and pretty much everything will work that works in Linux.

I don't regularly use OS X, but feel free to post questions as
issues - I or a fellow Mac user can probably help.

Installing FFMPEG with brew
===========================

@filangel suggests [how to install ffmpeg using brew](https://github.com/HPCE/hpce-2017-cw1/issues/33):

    brew install ffmpeg --with-fdk-aac --with-freetype --with-libass

It was also noted by @nar213: It can take up to 10 minutes.
