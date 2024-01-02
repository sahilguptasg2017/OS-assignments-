# Assignment 1

## Instructions
- There should be ONLY ONE submission per group.
- Submit a .zip named RollNo1_RollNo2.zip file containing code and write-up.

---

## Q1: Parent-Child Relationship and System Calls (Total points: 30)

Create a parent-child relationship between two processes. The parent should print two statements:

A) Parent (P) is having ID \<PID\>

B) ID of P’s Child is \<PID_of_Child\>

The child should print two statements:

C) Child is having ID \<PID\>

D) My Parent ID is \<PID_of_Parent\>

Make use of wait() in such a manner that the order of the four statements A, B, C, and D is: A, C, D, B.

Write a program to create two processes using vfork() system call in which the child process will calculate the factorial of 4, and the parent process will calculate the Fibonacci series up to 16. Parent should wait for the child to complete its working.

**Clarification:** For Fibonacci series, you have to calculate and print the first 16 elements of the Fibonacci series.

*Bonus part:*

Write a program to create two processes using vfork()/fork() system call in which the child process will calculate the factorial of 4, and parent process will calculate the Fibonacci series up to 16. The child should wait for the parent to complete its working.

### Rubrics:

- Program source code(s) with Makefile to compile.
- Write-up giving a brief description of how the program works (less than 1 page).
- Error handling.

---

## Q2: Custom Unix Commands (Total points: 50)

1) **word:**
   - It is a built-in (internal) command, Reads the number of words (assume word is a “space-separated” string) in a text file, and throws an error if the file does not exist.
   - Syntax: `word [-option] [file_name]`
   - Options:
     - `-n`: ignores new line character
     - `-d`: difference between the word sizes of two text files

2) **dir:**
   - It creates a directory, and then changes the path to that directory.
   - Syntax: `dir [-option] [dir_name]`
   - Options:
     - `-r`: removes if the directory already exists and create a new directory instead of throwing an error
     - `-v`: print a message for each step in the running of this command

3) **date:**
   - It returns the last modified date and time of a particular file.
   - Syntax: `date [-option] [file_name]`
   - Options:
     - `-d`: display time described by STRING
     - `-R`: output date and time in RFC 5322 format

### Rubrics:

- Program source code(s) with Makefile to compile.
- Write-up giving a brief description of how the program works (less than 1 page).
- Use C - libraries for implementing the shell commands.
- Use of exec(), fork(), wait().
- Error handling in terms of wrong command or wrong option or wrong argument.

---

## Q3: Arithmetic Calculator Bash Script (Total points: 20)

Make an arithmetic calculator for a school following the given instructions:

- Create a bash script shell that acts like a math calculator.
- Read a text file named “input.txt” that has two numbers and an operation in the format x y operation where x and y are numbers and the operation is the name of the command.
- Calculate the result of that operation.
- Save the result in a new text file named “output.txt” in the directory named "Result" (if the directory doesn't exist in the current directory, make it).

**Operations:**
1. "xor": Get the xor of the two given numbers.
2. "product": Get the product of the two given numbers.
3. "compare": Get the bigger number from the two given numbers.

### Rubrics:

- Program source code(s) with Makefile to compile.
- Write-up giving a brief description of how the program works (less than 1 page).
- Read, write of text file through bash script only.
- Creation of directory(if needed) through bash script only.
