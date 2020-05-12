COP4610 Project 3 FAT32 README
------------------------------

Contributors
------------
Tate Bacaner tb15j
Jason Fiegle jaf16d

Description
-----------
Project3 pdf contains information about requirements and constraints for the project

File Listing
------------
main.c:
contains all of the code for commands to be used on the FAT32 image file

Makefile:
Creates the project3 executable to be used with image files

README.md:
Contains vital information regarding group members, file listing, Makefile description, documentation of group member contribution and known bugs section

Makefile Description
--------------------
Generates project3 from main.c
Compiled by typing make and cleaned by typing make clean

Bug Listings
------------
Reading across multiple clusters will work the first time but the second time gives a piece of garbage data between clusters while still able to finish the read. Any time after that for the same exact function call will vary sometimes being correct and sometimes incorrect. When running with valgrind no memory errors are detected and the previously stated bugs do not occur.

Documentation of Group Member Contribution
------------------------------------------
exit: Created and done by both Tate Bacaner and Jason Fiegle

info: Created and done by both Tate Bacaner and Jason Fiegle

size: Created and done by both Tate Bacaner and Jason Fiegle

ls: Created and done by both Tate Bacaner and Jason Fiegle

cd: Created and done by both Tate Bacaner and Jason Fiegle

creat: Created and done by both Tate Bacaner and Jason Fiegle

mkdir: Created and done by both Tate Bacaner and Jason Fiegle

mv: Created and done by both Tate Bacaner and Jason Fiegle

open: Created and done by both Tate Bacaner and Jason Fiegle

close: Created and done by both Tate Bacaner and Jason Fiegle

read: Created and done by both Tate Bacaner and Jason Fiegle

write: Created and done by both Tate Bacaner and Jason Fiegle

rm: Created and done by both Tate Bacaner and Jason Fiegle

cp: Created and done by both Tate Bacaner and Jason Fiegle
