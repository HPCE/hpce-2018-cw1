Coursework 1
============

This is a fairly simple coursework that is designed to:

- Get everyone up and running with some command line tools.

- Start to think about measuring performance and making disciplined comparisons

- Getting some basic speed-ups using `tbb::parallel_for`.

- Starting to think about how parallelism can have different
  effects depending on where it is added.

The coursework is due:

    22:00 Mon 28 Jan

Submission is via blackboard for this coursework, as it is
unfair to ask everyone to fully use git so soon. Future
courseworks will be via git only.

At various points I mention posting "issues". Issues are a way
of registering bugs, but I also use them here for communication.
If you want to register an issue, go to the [issues](https://github.com/HPCE/hpce-2018-cw1/issues)
page for the shared master repository. Please note that issues
should include enough information to allow other people to help.

Useful things to include in an issue are:

- Descriptions of what you are trying to achieve

- Some idea about what is going wrong

- Any alternatives you have tried

- Error messages

- Screenshots

This coursework is in its second year. The previous version
used matlab, and seemed to dismay people, so I finally gave
up and went for C++ from the start. It's relatively simple
once you have an environment, so is mainly intended for
an early performance win.

Choose a platform
=================

You can use any platform you want for development, as long as it
has more than one CPU, and supports a unix-like flow.

Specific tools you'll want installed are:

- [A unix-like shell](https://en.wikipedia.org/wiki/Unix_shell) :
  I recommend Bash, which is what will turn up as the default on most platforms.
  If you are a hipster then something like [zsh](http://ohmyz.sh/) is fine I suppose.

- A c++ compiler : I recommend g++, but clang is fine too. Whatever
  it is, it should be reasonably up to date, as we need c++11 support.

- [make](https://en.wikipedia.org/wiki/Make_(software)) :
  [GNU make](https://www.gnu.org/software/make/) is usually the best
  option, though other variants will probably work.

- [git](https://git-scm.com/) :
   The source control tool underlying github.

Reasonable choices are (in no particular order):

- [Linux](readme_linux.md)

- [Windows](readme_windows.md)

- [OS-X](readme_os_x.md)

See the linked details for setup discussion on each.

Clone the source code
=====================

Assuming you have sent me your login details, you should
have your own private repository containing a copy of
the coursework. If you are currently on the github website
looking at:

    https://github.com/HPCE/hpce-2018-cw1

then you are on the shared (read-only) copy of the specification.
If you are looking at:

    https://github.com/HPCE/hpce-2018-cw1-[YOUR_LOGIN]

then you are at your own private copy. No-one else should be
able to see this version, except for me.

Notice that there is a green button called "Clone or Download"
on the top right of the page. If you click it and choose `https`,
you should get the following URL:

    https://github.com/HPCE/hpce-2018-cw1-[YOUR_LOGIN].git

Open your terminal, and navigate to a convenient directory
(e.g. your working directory, or some directory containing
all your courseworks). You can now clone a local copy of your
repository by doing:

    git clone https://github.com/HPCE/hpce-2018-cw1-[YOUR_LOGIN].git

After typing in your github credentials, you should have
a local copy of the repository. Note that you could also use
a git GUI for this step - it doesn't really matter how you get it.

To move into the repository, do:

    cd hpce-2018-cw1-[YOUR_LOGIN]

If you do `ls`, you should see all the files and directories
from the repository.

_*Note* : Almost all terminals support [tab completion](https://en.wikipedia.org/wiki/Command-line_completion),
so if you type `cd hp`, then hit tab, it will either auto-complete the full path, or will
show all paths starting with that prefix._

Get the code building
=====================

Use the makefile provided to build the code:

    make bin/julia

Try running the code:

    bin/julia

You should see a strange ASCII pattern come out (it
may come out faster or slower, depending on your
machine). This is a rendering of the [Julia set](https://en.wikipedia.org/wiki/Julia_set).

_*Note* : Most terminals allow you to recalle previous commands by using the "up" key._

At this point you may wish to play around with the parameters:

    bin/julia -help

_*Note* : if you want to kill the application, then the standard console command is Control+c_

For example, you can make it show an animation:

    bin/julia -animation

You can change the width and height:

    bin/julia -width 100 -height 60

You can make the problem easier, by varying the number of iterations:

    bin/julia -max-iter 100

### Rendering as a video (optional extra)

If you install [ffmpeg](https://www.ffmpeg.org/download.html) (or avconv on Ubuntu), then you can also
view it as a video. _@nar213 offers some [suggestions on how to install ffmpeg on OS X](readme_os_x.md#ffmpeg)._

First, make it render as a file containing raw RGB32 frames:

    # Render 50 frames as a raw rgb32 video to a file called julia.bin
    bin/julia -max-iter 100 -max-frames 50 -width 640 -height 480 -video > julia.bin

To convert it to an `.mp4` you can use ffmpeg (or avconv):

    # Convert it into an mpeg 4 file called julia.mp4
    ffmpeg -f rawvideo -framerate 25 -pixel_format rgb32 -video_size 640x480 -i julia.bin -vcodec libx264  -pix_fmt yuv420p julia.mp4

If you have a GUI available (e.g. in Linux or OS-X) you can play the video directly using ffplay (or avplay):

    # Play the file back using ffmpeg
    ffplay -f rawvideo -framerate 25 -pixel_format rgb32 -video_size 640x480 julia.bin

You can also avoid the optional intermediate file `julia.bin` using a [pipe](https://en.wikipedia.org/wiki/Pipeline_(Unix)):

    bin/julia -max-iter 100 -width 640 -height 480 -video   |   ffplay -f rawvideo -framerate 25 -pixel_format rgb32 -video_size 640x480 -

Because there is no intermediate file, the piped version can play forever.

Establishing current performance
================================

Currently the default julia is probably quite slow (though it
depends on the computer and OS). We want to make it faster, so
first we need to establish how fast it currently is. There is a
builtin command in bash (and other unix shells) called `time`:

    time bin/julia

You should see some output that breaks the execution time down into:

 - *real* : the amount of wall-clock time taken to execute

 - *user* : the total amount of CPU time taken executing the actual program,
   across all CPUs.

 - *sys* : the amount of OS/kernel time taken on behalf of the program

 _**Note**: it turns out that `time` in mingw64 is broken,
 [and returns incorrect values for user and system time](https://github.com/HPCE/hpce-2018-cw1/issues/37).
 There is a workaround detailed in the issue to get correct user time, but note that the
 remaining parts only really need execution time (i.e. the "real" part)._

You may find that `real = user + sys`, or `real > user + sys`. Eventually
we would like to get to `real = user / P + sys`, where P is the number of
processors (e.g. see [this question](https://github.com/HPCE/hpce-2018-cw1/issues/26)).

Note that in the following I have no particular hardware or
software platform in mind - it is whatever you prefer to use.

As you're doing these experiments, you may wish to supress the
printing of the fractal. One way of doing this is to redirect
the output to the [null device](https://en.wikipedia.org/wiki/Null_device):

    bin/julia > /dev/null

### Scaling with max iter

One way that the performance can vary is with the maximum number of iterations
per pixel.

**Task**: explore the relationship between maximum iteration count (x) and execution time (y),
plot it as a graph, and save it as `results/max_iter_versus_time.pdf`.

Exactly how you vary the maximum iteration count is up to you, as is the
maximum iteration count considered. The only idea here is to get an idea
of the shape of the scaling curve, so don't worry about having hundreds
of data-points - 7 or 8 is probably enough, as long as you cover a low
maxiumum iteration count (e.g. 1) up to a reasonably long one (e.g. around 10 seconds or so).

I would suggest using excel or open-office to record the data, as you'll
want it handy to compare against later results. You can then print or export
the graphs to the pdf.

### Scaling with width and height

Do the same thing, but explore the scaling relationship as image dimensions
change. Take the independent parameter as the scale N, and look at execution
times for width x height = N x N.

**Task**: explore the relation between the scale N (x) and execution time (y),
and save it as a graph `results/dimension_versus_time.pdf`.

### Look at the code, and try to explain the scaling

So far the code has been a black-box, but it is now worth trying
to understand the overall structure. It is generally a good
idea to look at programs top-down, starting from the headers.

In this case, the key files to look at are:

 - [include/julia.hpp](include/julia.hpp) : this gives the high-level interface
    between the driver (i.e. the user interface), and the engines (the parts that
    do the calculation).

 - [src/julia_driver.cpp](src/julia_driver.cpp) : this is the user interface, and
    manages command line options as well as the output mode (e.g. text output versus binary video).

 - [src/julia_frame_reference.cpp](src/julia_frame_reference.cpp) : this is the
    default rendering engine, i.e. the part that renders the fractal.

It is worth briefly following the logic of the code through the `main` function
in `src/julia_driver.cpp` just so you have an idea of what is going
on. Notice that there is some wierdness in `RenderProgressive` with an
extra thread - this is what allows the GUI to render the pixels as they
are drawn. The indirection via `juliaEngine` may also be confusing. However,
confusing initialisation and GUI code is normal; what you are looking for
is the computational bottleneck.

If you go into `src/julia_frame_reference.cpp`, then things get much simpler.
For now I will simply assert that this is the computational bottleneck. In
real life, it is usually up to you to find it for yourself in a large code-base.
Notice that there are three nested loops here:

- Do they explain the scaling behaviour seen in your graphs?

- Can you explain the graph for small image dimensions and for small maximum iterations?

Some of you will have done algorithmic complexity in 2nd year, or have
encountered it elsewhere. Here we're seeing the implications
of [big-O](https://en.wikipedia.org/wiki/Big_O_notation), and it is worth remembering
that in all that follows in this exercise we are not changing the fundamental
complexity of the problem, only the scaling constant that goes in front of it.

Commit and push back to github
------------------------------

_Don't worry if this doesn't work immediately. If you get stuck
then post an issue._

You have now added some files to your solution, and so probably
want to make sure those files get back into the repository. There
are three stages to this:

 - "Add" : tell git that you want it to track a file

 - "Commit" : tell git that the specific version of the file should be captured

 - "Push" : upload the committed changes back to github.

If you have a GIT GUI, then you can probably right click the folder
and choose "Commit", then "Push" to do these steps.

On the command line, the steps are:

    # Add the pdfs you created
    git add results/*.pdf

    # Commit the files, with the message "Added graphs"
    git commit --all -m "Added Graphs"

    # Push the changes back up (you'll need to type your password)
    git push

If you hit refresh on your private repository on the github
website, you hopefully will see that the graphs have appeared in github.

Turning on optimisations
========================

This is a really obvious one, but over the years I've come to realise that
some people don't know it, or don't actively think about it. So:

- Compilers can do huge amounts of clever optimisations.

- *But*, by default, compiler optimisations are usually turned off.

In previous years I've had students submit things for their final
coursework which were wonderfully parallel on 36 cores, but had
no compiler optimisations turned on.

Exactly how you enable optimisations depends on the compiler and
platform. In GUIs like Visual Studio and Eclipse there is usually
a "Release" build (as opposed to the default "Debug" build). In
build tools like cmake there is a similar option for targetting
release. For low-level tools like `make`, it is up to you to enable
it.

**Task**: Look through the makefile for a line which looks like:

    # CPPFLAGS += -DNDEBUG=1 -O3

and remove the `#`. The `#` comments out the line, so by removing
it you are adding two things to the `CPPFLAGS` variable:

- `-O3` : Turn on compiler optimisations at the highest level. Alternatives
    would be `-O1` and `-O2`, which perform less aggressive optimisations.
    Historically `-O3` was seen as risky due to broken compiler optimisations,
    but this is very uncommon these days.

- `-NDEBUG=1` : This is a less commonly known one, but adds a
    pre-processor definition to the build, equivalent to `#define NDEBUG 1`.
    The effect is to supress the evaluation of [`assert`](http://www.cplusplus.com/reference/cassert/assert/) statements.
    Including `assert` statements is incredibly valuable when debugging, but
    once the code is correct they are only slowing things down <sup>[2](readme_footnotes.md#assert_in_production)</sup>.

Modifying the makefile won't tell `make` that the inputs have changed; to
do that you can pass the `-B` option to make, or touch/modify one of the sources.

**Task**: Re-generate the maximum iteration scaling performance from earlier,
and plot a graph of the absolute release execution times *and* the absolute debug execution times
(seconds on the y scale). Save it as a graph called `results/release_max_iter_versus_time.pdf`.

**Task**: Re-generate the dimension scaling performance from earlier,
and plot a graph of the release execution *speed-up* compared to the debug execution times
(speed-up on the y scale). Save it as a graph called `results/release_dimension_versus_speedup.pdf`.


You may want to look in detail at your graphs, and consider what is happening
at the smaller/faster end of the scale. log-log scales are often a good
way of looking at absolute times, and log-linear for speedups.


Making things parallel
======================

We are now going to make things parallel using TBB, using one of the
simplest primitives: [`tbb::parallel_for`](https://software.intel.com/en-us/node/506153).
We're also going to use the simplest form of `tbb::parallel_for`, which looks
the most like a for loop. Other forms are more efficient, but require more
rewriting.

The basic template we will follow is to transform this:

    for(unsigned i=begin; i < end ; i++){
        // Code that calculates a function dependent on i
        f(i);
    }

into this:

    #include "tbb/parallel_for.h"

    tbb::parallel_for(begin, end, [&](unsigned i){
        // Code that calculates a function dependent on i
        f(i);
    });

There is a file called [`src/julia_frame_parallel_inner.cpp`](src/julia_frame_parallel_inner.cpp),
which contains a stub for a function called `juliaFrameRender_ParallelInner`.
You can select this rendering engine by doing:

    bin/julia -engine parallel_inner

At the moment this will fail, for obvious reasons.

**Task**: copy the reference implementation from `juliaFrameRender_Reference` into `juliaFrameRender_ParallelInner`,
and parallelise the *inner* loop using `tbb::parallel_for`.
The innner loop is simply the inner-most loop in the heirarchy of loops, in this case
the loop over `x`.

When first compiling with TBB, you are likely to see the two stages of errors we
encountered in the in-lecture demo:

- Compilation errors : usually due to a missing `#include`, or due to the TBB
  files not being on the include path. You may also see problems due to [the
  argument types for `parallel_for`](https://github.com/HPCE/hpce-2018-cw1/issues/35).

- Linker errors : this is because the TBB library needs to be linked into the
  executable.

If you use a version of TBB installed with your package manager, the only
change you should need is to uncomment this line in the makefile:

    # LDLIBS += -ltbb

by changing it to:

    LDLIBS += -ltbb

i.e. removing the '#'. This is the equivalent of when `-ltbb` was explicitly
added to the command line in the in-lecture demo. _Thanks to @ppp2211 for
[pointing out this was missing](https://github.com/HPCE/hpce-2018-cw1/issues/32)._

_**Note**: you may find that the platform you originally chose makes it difficult
to install TBB. Do not despair - your code should be portable to other platforms,
so you can always change: [Linux](readme_linux.md), [Windows](readme_windows.md), [OS-X](readme_os_x.md)._

Once you have got this working, you should play around with the resulting change
in performance. It should be asymptotically faster than the reference version,
but you will likely find that for small scale parameters (max iterations and
image dimensions) there is less speed-up, or even a slow-down.

Making things parallel (again)
==============================

Hopefully you have seen a speed-up, but (as is usually the case), there
is more than one place where we can add parallelism. Often one of the
difficulties is deciding which part to parallelise.

**Task**: Create an implementation called `juliaFrameRender_ParallelOuter` in
[`src/julia_frame_parallel_outer.cpp`](src/julia_frame_parallel_outer.cpp) which
parallelises the *outer* loop.

Again, experiment a bit with the flag `-engine parallel_outer` to see how performance changes.
You may want to consider varying the width versus height, and see how it compares against parallel_inner.

Making things _super_-parallel
==============================

Ok, if we can make things parallel on the outer loop or on the inner loop,
surely doing it on both is an even better idea!

*Task*: Create an implementation called `juliaFrameRender_ParallelBoth` in
[`src/julia_frame_parallel_both.cpp`](src/julia_frame_parallel_both.cpp) which
parallelises both loops.

Evaluating the speed-up
=======================

We now have four implementations:

1 - Reference

2 - Parallel Inner

3 - Parallel Outer

4 - Parallel Both

They should all be correct (I'm not aware of any gotchas which would make them break,
like shared variables), so the main concern is - _which is fastest?_ This
questions is impossible to answer without knowing something about the scale
of the input parameters, and even then it is highly dependent on the platform.

I'm going to ask you to sweep five parameters, in order to explore
scaling along different axes (axis?). It is not expected that any
one implementation should be best in all cases, but some general
principles should emerge. Also, be sure to include some points
close to 1 - even though we are unlikely to run with width=1 or
height=1, it is conceptually quite useful to be able to explain
what happens in these cases.

For each of the implementations you will produce a graph, and it
is up to you exactly what that graph looks like. However, the
graphs should:

- Be correctly labelled (what are the axis? What is the title?)

- Compare all four implementations in some meaningful way (consider
  the two ways I've suggested so far).

- Communicate something (can I look at it and understand the message?)

- Be vector graphics, not bitmap<sup>[3](readme_footnotes.md#vector_graphics)</sup>

I would also encourage you to include the raw data in the `results` directory
(e.g. as `.xls` or `.csv` or whatever), as it is generally a good idea
to keep hold of the original data, as well as the graphs. Also, feel free
to include other graphs, or include graphs for other experiments. If you
want to add more implementations, that is fine as well (as long as the
existing ones still work).

For each of these experiments, use base-line parameters of:

- width = 512

- height = 512

- max-iter = 512

- max-frames = 1

**Task**: Evaluate the scaling against max-iter (x-axis) with the baseline width, height, max-frames,
    and save it as `results/scaling_max_iter.pdf`.

**Task**: Evaluate the scaling against dimension (x-axis), where dimension (N) is defined the
    same way as the earlier experiment (image size is NxN) and save it as `results/scaling_dimension.pdf`.

**Task**: Evaluate the scaling with width, and save as `results/scaling_width.pdf`.

**Task**: Evaluate the scaling with height, and save as `results/scaling_height.pdf`.

**Task**: Evaluate the scaling with max-frames, and save as `results/scaling_max_frames.pdf`.
  Note: this requires the `-animation` flag to be passed in order to see differences.

The whole goal of getting you to do these graphs is to get you to think about
what is happening. Even without knowing how TBB is doing things, how can
you explain what happens? When is `parallel_inner` good versus when is `parallel_outer`
good? Under "normal" parameters, which method would you recommend using?

Submission
==========

I highly recommend that you keep your github repository updated as you
go along, as if you do that then you will occasionally get updates on
whether your code works in the target environment. However, for this submission
we will actually use blackboard to submit.

In order to submit:

- run `make clean` to get rid of temporary files.

- zip your submission directory into a file called `[YOUR_LOGIN].zip`.
  Note, this zip should [contain the sources as well](https://github.com/HPCE/hpce-2018-cw1/issues/28).

- Submit it via blackboard.

Note that you have unlimited uploads to blackboard.

The new [late submission policy](https://www.imperial.ac.uk/media/imperial-college/administration-and-support-services/registry/academic-governance/public/academic-policy/marking-and-moderation/Late-submission-Policy.pdf)
means that coursework submitted up to 24 hours late will still be marked, but is capped at 40%.
Because submission is unlimited, someone could submit both before and after the deadline; as a consequence
the following rules will apply:
- The deadline is 22:00 Fri Oct.
- There is a grace period from 22:00-22:10 to allow for latency uploading to black-board.
- Any submision with a time-stamp after 22:10 will be considered a late submission, even
  if there are earlier submissions.
Please be aware of the potential for slow uploads to blackboard, especially if you
are trying to upload a very large zip (which ideally you won't need to).

### Submission script

A submission test script is now included. Once you
have created a zip file, you can test it using:

    ./test_submission.sh [YOUR_LOGIN] [PATH_TO_YOUR_ZIP]

It relies on a few tools such as `awk`, `unzip`, and `grep`,
but it should warn you if it doesn't work.
