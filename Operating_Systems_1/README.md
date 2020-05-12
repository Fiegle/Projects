COP4610 Project 1 Shell Project README
--------------------------------------

Contributors
------------
Tate Bacaner tb15j
Jason Fiegle jaf16d

Description
-----------
This project serves to emulate bash shell done from C code with the capability to execute unix commands with piping, I/O redirection and other unix implementations as can be seen from the Documentation of Group Member Contribution Section.

File Listing
------------
main.c  : Contains every part of the project from the division of labor. 
pathutil.c  : Used for path resolution when a /../ or /./ are involved.
pathutil.h  : Header file for path resolution when a /../ or /./ are involved.
Makefile  : creates the executable shell, detailed description of Makefile below. 
README.md : Contains vital information regarding Group members, file listing, makefile description, documentation of Group                            Member Contribution and known bugs section.
commitlog.txt : Contains a log of all commited changes to the GitHub project.

Makefile Description
--------------------
Generates pathutil.o from pathutil.h and .c object files
Generates main.o from main.c
Creates executable shell from main.o and pathutil.o
Compiled by typing make and cleaned by typing make clean.

Bug Listings
------------
Errors with Putty and Piping: When running multiple pipe commands on Putty corrupted size vs. prev_size. This error does not happen when using Tectia SSH. The cause is most likely due to our implementation of Dynamic Allocation. The command that threw the error could be executed by rerunning the shell.

Documentation of Group Member Contribution
------------------------------------------

Parsing: Done mostly from provided file parser_help.c with added built-ins.
Contributing members: Created by both Tate and Jason.

Environmental Variables: Sifts through the list of tokens and checks to see if there are any with $ in front. If so if the following string exists then the token is converted to its environmental variable.
Contributing members: Created by Jason, reviewed by Tate.

Prompt: Issued when the shell is active and ready for commands to be entered.
Contributing members: Created by both Jason and Tate. (Jason worked on the working directory and Tate worked on the USER@MACHINE)

Shortcut Resolution: Path conversion to absolute path names. Done when a user enters custom shortcuts such as .., ~, ., etc.
Contributing members: Created by Tate and reviewed by Jason.

$PATH Resolution: Check for executable files in all available paths. If an executable for the command is found, assign that path to the token.
Contributing members: Created by Tate and reviewed by Jason

Execution: Forking into a child process to then get the command executables and execute. We added I/O redirection and piping within our execution function.
Contributing members: Created and worked on by both Tate and Jason

I/O Redirection: when a < or > or combination are used to change the descriptors from input or output based on what is passed in.
Contributing members: Created by Jason and reviewed by Tate.

Piping: Changing the output of the initial command(s) to the following command(s) while also changing the input from the following command(s) to be from the previous command(s). We successfully implemented a single pipe however were unable to get 2 pipes to function properly. Two pipes do no throw any errors or seg faults but do not output. Most likely an issue from either not closing a part of the pipe or incorrectly assigning a file descriptor to STDOUT_FILENO or STDIN_FIILENO.
Contributing members: Created by Jason and reviewed by Tate.

Background Processing: Execution of child processes while not inhibiting the running of the parent process, denoted by &. We were unable to implement proper background processing so unfortunately we left it out of the project.
Contributing members: NA

Built-Ins:
Exit: Exit will take us out of the shell unless there are background processes.
Contributing members: Created by Tate reviewed by Jason
          
cd: cd will change the directory based on the directory entered by the user. Our cd checks for errors of too many arguments or invalid path.
Contributing members: Created by Jason reviewed by Tate.

echo ARGS: will check to make sure every argument passed is valid; if there is a single invalid argument an error is thrown that matches the bash shell. Otherwise all arguments are echoed with a space between them and all environmental variables are changed to their correct path names.
Contributing members: Created by Jason reviewed by Tate.

jobs: outputs our background processes. Unfortunately we have no background processing implementation therefore jobs will output there are no jobs.
Contributing members: Created by Tate reviewed by Jason.
