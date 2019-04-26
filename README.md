# Linux Minishell
A mini Linux shell that can interpret and execute the commands that are typed in 
interactively or included in batch files. The shell is designed to be able to execute commands
concurrently in order to finish tasks more quickly.

***

The shell can work in an interactive mode or a batch mode depending on how the shell is launched.

### Interactive Mode
If the shell is launched without additional arguments, it works interactively, executing commands as they are entered. The shell creates a child process that executes the command the user entered. By default, when the child process has finished, the shell shows the prompt for more user input. The shell also supports background commands: if a command is ended with a `&`, the  shell does not need to wait for the command to finish executing. Instead, it shows the prompt and is ready to accept new commands after it has created the child process.

### Batch mode
If the shell is launched with the pathname of a batch file specified in the command line, it works in batch mode. The batch file contains the list of commands that should be executed, one command on each line. The functionality of the shell in batch mode is essentially the same as that in interactive mode: read a command, spawn a child process to execute the command, wait for the finish of the child process if the command is not  put in background, and repeat the above procedures for other commands. 

***

In addition  to  normal  command  lines  (e.g., those  running existing Linux tools), the shell also responds to the following two special commands (in both interactive mode and batch mode): 

- `quit`: The shell stops accepting new commands when it sees the quit command on a line or reaches the end of the batch file. The shell should then exit after all running processes have terminated. 

- `barrier`:  In the interactive mode, when the shell sees the barrier command, it stops accepting new commands until all running processes have terminated. It does not show the prompt either during the waiting. In the batch mode, when the shell sees the barrier command, it stops reading new commands until all running processes have been terminated. This command cannot be executed in the background. The `&` in `barrier&` is ignored. 

***

Included is a batch file to test the batch mode functionality. It analyzes Wikipedia pages to find out the most frequently used words on those pages. The execution of the batch file generates a list of distinct words used in the web pages and the number of occurrences of each word on these web pages sorted by the number of occurrences in ascending order.
