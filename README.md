# Linux-shell
Linux shell implemented in C.


On every line a prompt is displayed when waiting for user input. The prompt is in the format `<Username@SystemName:~>`.
The directory from which shell is executed becomes the home directory for the shell and represented with “~”. When user changes the working directory, the corresponding change in path of directory is reflected in the next prompt. 

### Input

Commands to the shell can be separated by ';' and '&'. Their use is similar to Bash syntax. Commands separated by ';' run in sequence. These commands run as foreground processes that have control of the shell terminal for the duration of execution. Commands terminated with '&' run as a background process. To execute a background process, shell creates another process (which does not have control of the terminal) and prints its PID. The terminal can continue receiving and executing prompts.

### I/O Redirection

The use of '>', '<', '>>' is supported for I/O direction.

### Pipes

The use of pipes ('|') to use the output of one command as the input of the next is supported.

I/O redirection and pipes work together.

The list of implemented commands are explained below. Apart from these, the shell can execute the other system commands present in Bash like emacs, gedit etc. This is possible for both foreground and background processes.

### warp

‘warp’ is similar to 'cd' in Bash. It changes the directory that the shell is currently in. It also prints the full path of working directory after changing. The directory path/name can be provided as argument to this command. If no argument is provided, the directory changes to the home directory.

The “.”, “..”, “~”, and “-” flags are implemented for warp (their use is similar to that in the cd command in Bash).

Assumption: path names do not have white spaces.

### peek

**peek <flags> <path/name>**

‘peek’ command lists all the files and directories in the specified directories in lexicographic order. The -a and -l flags are supported.

- -l : displays extra information
- -a : includes hidden files in listing

Similar to warp, “.”, “..”, “~”, and “-” symbols are supported.

### pastevents commands

**pastevents**

The ‘pastevents’ command is similar to the 'history' command in bash. The implementation stores (and outputs) the 15 most recent command statements given as input to the shell based. `pastevents` is persistent over different shell runs. (commands that include `pastevents` are not stored.)

**pastevents purge**

Clears all the pastevents currently stored.

**pastevents execute <index>**

Executes the command at position in pastevents (ordered from most recent to oldest).

### proclore

proclore is used to obtain information regarding a process.

The information printed is:

- pid
- Process Status (R/R+/S/S+/Z)
- Process group
- Virtual Memory
- Executable path of process

### seek

**seek <flags> <search> <target_directory>**

‘seek’ command looks for a file/directory in the specified target directory (or current if no directory is specified). It returns a list of relative paths (from target directory) of all matching files/directories (files in green and directories in blue).

**Flags :**

- -d : Only look for directories and not files
- -f : Only look for files and not directories
- -e : If a single file or a single directory with the name is found, the contents of the file/directory will be printed if permissions allow.

### activities

This command prints all the processes that were spawned by the shell that are currently running. The information printed is:

- Command Name
  
- pid
  
- state : running or stopped
  

### Signals

**ping <pid> <signal_number>**

This command is used to send signals to specific processes.

Following 3 commands are direct keyboard input where Ctrl is Control key on keyboard (or it’s equivalent).

**Ctrl - C** interrupts the currently running foreground process, if any, by sending it the SIGINT signal.

**Ctrl - D** kills all processes currently running in your shell and exits the shell program.

**Ctrl - Z** the currently running foreground process is moved to the background and its state is moved from running to stopped.

### fg and bg

**fg <pid>**

This brings the background process with id <pid> to the foreground. This process must be a background process spawned by the shell.

**bg <pid>**

This changes the state of the given background process from stopped to running.

### Neonate

**neonate -n [time_arg]**

The command prints the Process-ID of the most recently created process on the system, this pid will be printed every [time_arg] seconds until the key ‘x’ is pressed.

## iMan

**iMan <command_name>**

iMan fetches man pages from the internet ([http://man.he.net/](http://man.he.net/)) and outputs it to the terminal (stdout).
