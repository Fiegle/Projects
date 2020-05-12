COP4610 Project 2 Elevator Project README
--------------------------------------

Contributors
------------
Tate Bacaner tb15j
Jason Fiegle jaf16d

Description
-----------
Check project2_description for information on constraints and requirements for this project

File Listing
------------
Part 1/empty.c:
Contains a skeleton main to test empty code with strace.

Part 1/empty.trace:
Contains all the system calls that were called when running strace on empty.c.

Part 1/part1.c:
Contains our code that executes 5 more system calls than in empty.c

Part 1/part1.strace:
Contains all the system calls that were called when running strace on part1.c.

Part 2/my_timer.c:
Contains the kernel module with proc_read for accessing current kernel time and elapsed time to be printed to the user.

Part 2/Makefile:
Creates the kernel object my_timer that can be used with insmod and rmmod as well as proc creation.

Part 3/elevator.c:
Contains all the necessary kernel modules, system calls, proc creation, threads, and functions for the elevator. 

Part 3/Makefile:
Creates the kernel object elevator that can be used with insmod and rmmod as well as proc creation.

README.md:
Contains vital information regarding Group members, file listing, makefile description, documentation of Group Member Contribution and known bugs section.

Makefile Description my_timer
-----------------------------
Generates my_timer.ko from my_timer.c  
Compiled by typing make and cleaned by typing make clean.

Makefile Description elevator
-----------------------------
Generates elevator.ko from elevator.c to be ran with commands

Bug Listings
------------
Sometimes our elevator will say loading when it moves to another floor without needing to pick people up.

Documentation of Group Member Contribution
------------------------------------------

Part 1: Created and done by Tate Bacaner

Part 2: Created and done by Jason Fiegle

Part 3: Equally done by both Tate Bacaner and Jason Fiegle
