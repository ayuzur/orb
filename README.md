# orb

an orb that explodes when a given time is reached

## building
to get a binary
1. clone repo
2. ensure ncurses, make, and gcc is installed 
    - (if you are on mac you may need to remove the w from "ncursesw" in the makefile)
3. run `make`

the `orb` binary will be placed in `./bin/`

## usage

To use `orb` pass it a command line arguement in the format of hh:mm:ss.
Ex: `./orb 00:00:10` to set the timer to 10 seconds

hit q to exit early
