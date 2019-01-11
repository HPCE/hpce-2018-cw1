
# Turn on warnings and C++ 11 features
CPPFLAGS += -W -Wall -std=c++11

# Turn off "unused parameter" in g++
CPPFLAGS += -Wno-unused-parameter

# Add the ./include directory to the header search path
CPPFLAGS += -I include

# Make it run fast
# CPPFLAGS += -DNDEBUG=1 -O3

# Enable pthreads
CPPFLAGS += -pthread

# Link in the TBB libraries
# LDLIBS += -ltbb


# Define the sources for the various julia engines
JULIA_ENGINE_SRCS = \
	src/julia_frame_reference.cpp \
	src/julia_frame_parallel_inner.cpp \
	src/julia_frame_parallel_outer.cpp \
	src/julia_frame_parallel_both.cpp

# Explain how to create bin/julia from sources in $(JULIA_SRCS)
bin/julia : $(JULIA_ENGINE_SRCS) src/julia_driver.cpp
	mkdir -p bin
	$(CXX) -o bin/julia $(CPPFLAGS) $(JULIA_ENGINE_SRCS) src/julia_driver.cpp $(LDFLAGS) $(LDLIBS)

# Alternate curses based interface. This will only build if curses is available.
# You may need to do `sudo apt-get install libncurses-dev` or equivalent first.
bin/julia_curses : $(JULIA_ENGINE_SRCS) src/julia_driver.cpp
	mkdir -p bin
	$(CXX) -o bin/julia_curses $(CPPFLAGS) $(JULIA_ENGINE_SRCS) src/julia_driver_curses.cpp $(LDFLAGS) -lcurses $(LDLIBS)

# Hack to get some kind of time command that supports user time under mingw64
ifeq ($(OS),Windows_NT)
bin/mingw64-time : src/mingw64-time.cpp
	mkdir -p bin
	$(CXX) -O3 -DNDEBUG=1 $< -o $@
endif

# A "clean" target. Originally missing, thanks to @lynx5120 for pointing out it was gone
# The '-' preceding the command is to indicate that we don't mind if it fails.
clean :
	rm -f bin/julia bin/julia_curses
	
