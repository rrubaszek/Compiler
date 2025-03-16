# Compiler for custom imperative programming language

## Overview
This project is a compiler for a custom imperative language (with `.imp` file extension). The compiler translates `.imp` source files into `.mr` files, which can be executed using a dedicated virtual machine. The virtual machine is implemented in C++ and is located in the `maszyna_wirtualna` folder. The compiler itself is located in the `src` folder.

## Features
- Custom imperative programming language (description below).
- Compiler written in C++17 using **Flex** and **Bison** for lexical and syntactic analysis.
- Virtual Machine implemented in C++ to execute compiled `.mr` files.
- Both compiler and virtual machine are built using **Makefile**.

## Project Structure
```
project-root/
├── maszyna_wirtualna/  # Virtual machine implementation
│   ├── Makefile        # Build script for the VM
│   ├── ...             # Source files for the VM
├── src/                # Compiler implementation
│   ├── Makefile        # Build script for the compiler
│   ├── ...             # Source files for the compiler
├── testy/              # Tests for the project and already translated programs
├── README.md           # Project documentation
├── gramatyka.txt       # Description of language's formal grammar
```

## Custom Imperative Language (.imp)

The .imp language is a procedural, imperative programming language. 
### Program Structure:
Every program in this language consists of a main program block and optional procedure declarations.
```
PROGRAM IS
    declarations
BEGIN
    commands
END
```
Procedures
```
PROCEDURE p(args) IS
    declarations
BEGIN
    commands
END
```
All variables must be declared in the declarations section before they can be used. The declarations section appears before the BEGIN keyword in both the main program and procedures.

Arrays are declared using range notation
```
array_name[start:end]
```

The language supports the following commands:
- Assignment
```
identifier := expression;
```
- Conditional statements

```
IF condition THEN
    commands
ENDIF
```

```
IF condition THEN
    commands
ELSE
    commands
ENDIF
```
- Loops
```
WHILE condition DO
    commands
ENDWHILE
```

```
REPEAT
    commands
UNTIL condition;
```

```
FOR pidentifier FROM value TO value DO
    commands
ENDFOR
```

```
FOR pidentifier FROM value DOWNTO value DO
    commands
ENDFOR
```
- Procedure Calls
```
proc_call(args);
```
- Input/Output operations
```
READ identifier;
```
```
WRITE value;
```
### Example program:
```
PROCEDURE PrintArray(T array) IS
    i
BEGIN
    i := 0;
    WHILE i < 10 DO
        WRITE array[i];
        i := i + 1;
    ENDWHILE
END

PROGRAM IS
    numbers[0:9]
    i
BEGIN
    i := 0;
    WHILE i < 10 DO
        numbers[i] := i * i;
        i := i + 1;
    ENDWHILE
    
    PrintArray(numbers);
END
```

## Installation and Compilation
### Requirements
- C++17 compiler (e.g., `g++` or `clang++`)
- `flex` (for lexical analysis)
- `bison` (for syntax analysis)
- `make` (for building the compiler and virtual machine)

### Building the Compiler
```sh
cd src
make
```
This will generate the compiler executable.

### Building the Virtual Machine
```sh
cd maszyna_wirtualna
make
```
This will generate the virtual machine executable.

## Usage
### Compiling a `.imp` File
```sh
./compiler input.imp -o output.mr
```

### Running a Compiled `.mr` File
```sh
./virtual_machine output.mr
```

## License
*(Specify your license here, e.g., MIT, GPL, etc.)*

## Contribution
Feel free to contribute by submitting issues or pull requests!

