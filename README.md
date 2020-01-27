# Simple Piper
A simple program to run multiple Linux shell commands written in a text file as desired in three options.
Written for operating systems class assignment.

## Usage
To compile:
```./compile.sh```
To run:
```./SimplePiper [option] <file>```

## Options
To run commands in order, run the program with ```1``` option and seperate the commands in text file using new lines.
ex1.txt:
```
<command 1>
<command 2>
<command 3>
...
```

To run commands parallel, run the program with ```2``` option and  seperate the commands using semicolons.
ex2.txt:
```
<command 1>;<command 2>;<command 3>;...
```

To run commands in pipe, run the program with ```3``` option and  seperate the commands using vertical bars.
ex3.txt:
```
<command 1>|<command 2>|<command 3>|...
```
