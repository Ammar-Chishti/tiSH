# tiSH (Tiny Shell)

This is a mini-shell written in C that supports a subset of functionalities that a bash shell would support.

    * Commands `cd, pwd, echo, and exit` are implemented from scratch
    * Supports all other bash commands present in the current machine
    * Supports being run in interactive as well as non-interactive mode
    * Supports debug mode and time counting mode
    * Supports redirection of stdin, stdout, and stderr
    * Supports creating and reading variables, correctly inherits existing environment variables
    * Regression tests cover all of the functionality mentioned above

## How to compile and run tiSH
* NOTE - You must have glib installed on the machine in order to run this program! [Installing gtk and glib](https://stackoverflow.com/questions/5275196/)
* Type in `make` to compile all files and create the `tiSH` executable. You can also type in `make clean` if you want to clean the directory build and binary files

## tiSH interactive mode
* To run tiSH in interactive mode, run the tiSH executable (`./tiSH`). Here you will be prompted to type in commands and use tiSH as if it were a bash terminal
* Note that tiSH will not support commands longer than 4096 characters in size

## tiSH non-interactive mode
There are two ways to run tiSH in non-interactive mode
* One way is to supply the file containing the list of commands to execute as an argument to the tiSH executable (ex. `./tiSH commands.txt`)
* Another way is to create a file with the commands, insert a shebang as the first line with the path to the tiSH executable (ex. `#!/home/achishti/tish-folder/tiSH-exec-file`), mark the file as executable and execute it
* NOTE - If you run tiSH in non-interactive mode, make sure every line in your file ends with a newline (\n). See how the test files are structured to get an idea.

## Variables and Echo Support
* The `echo` cmd can print out not only text but also env variables and variables set by the user (ex. `echo $PATH`) or (ex. `FOO=bar \n echo $FOO`). This also includes the return status of a command `echo $?`
* A singly linked list is used to store all of the variable names and values

## Debugging Mode
* Starting tish with -d `./tish -d` will display debugging info on stderr
* Before every command, a message in the format `RUNNING: "<cmd>"` will be displayed.
* After the command is finished executing, the message `ENDED: "<cmd>" (ret=%d)"` will be displayed where %d is the return status of the command

## Time Counting Mode
* Starting tish with -t `./tish -t` will display usage statistics after a command finishes executing
* The statistics message will be in the format `TIMES: real=x.xxxs user=x.xxxxxxs sys=x.xxxxxxs

tiSH can be started with -t and -d in any order (ex. `./tish -d -t` or `./tish -t -d`)

## Regression Tests
Type in `make tests` to run the regression tests. Make sure you run `make` to generate the executable first however.
    
    * Test01.sh will test the functionality of built-in commands `cd, pwd, echo, and exit`.
    * Test02.sh will test all redirection functionality.
    * Test03.sh will test if certain variables are correctly set. Examples of this include if env variables are inherited properly, if a user defined variable was set properly, or the correct return value was set after the execution of a command

Note that the test files mentioned above not only test if the return value of a command was correct but also if the output was correct as well. If you look closely, there might be a few spaces inserted into the output str we are testing equality against in the .sh files. This is because bash will sometimes replace \n characters with a space. In general, parsing the output of bash commands is just a pain to work with.