THIS IS STILL UNDER HEAVY DEVELOPMENT. CHECK BACK FREQUENTLY FOR NEW FEATURES
Sash Shell By Alec Snyder with tremendous help from Sam Baugh

Build Instructions: Download the makefile and run 'make clean' then 'make'

To call the shell in interactive REPL mode, just run 'sash'
You can put a list of commands and run them in a script by passing the
commands in a file as an argument. For example: 'sash mycommandfile'

The current feature are normal shell operations, various commands like cd,
pwd, etc... and output redirection but not appending abilities yet. There is
support for multiple commands separated by semi-colons. Many
features will be coming soon, check back and pull the latest sources
frequently for the latest code. Also, most parsing errors, simply say "An error has
occurred" more detailed error messages coming soon. An error that says: "comm:
no such file of directory" or something of the like means that such a command
doesn't exist in the given PATH.

Questions, Comments, and Suggestions can be e-mailed to linuxbash8@gmail.com

Thanks for using sash!
Licensed under the BSD3 license.
