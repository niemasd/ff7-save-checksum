# use g++ compiler with C++11 support
CXX=g++
CXXFLAGS=-Wall -pedantic -O3
TOOLS=checksum

# check for Windows vs. Linux
ifdef OS
	CHECKSUMEXE = checksum.exe
else
	CHECKSUMEXE = checksum
endif

# compile all tools
all: $(TOOLS)

# compile checksum tool
numlist: checksum.cpp
	$(CXX) $(CXXFLAGS) -o $(CHECKSUMEXE) checksum.cpp

# remove all compiled files
clean:
	$(RM) $(TOOLS) *.o
