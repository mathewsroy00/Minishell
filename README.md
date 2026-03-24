🐚 MiniShell – Linux Shell in C
📌 Overview

MiniShell is a lightweight Unix-like shell implemented in C that supports command execution, piping, and job control. It demonstrates core system programming concepts like process management and signal handling.

🚀 Features
Built-in commands: cd, pwd, echo, exit, jobs, fg, bg
External command execution using execvp()
Pipe support (|) for multiple commands
Job control with foreground/background execution
Signal handling (Ctrl+C, Ctrl+Z)
Customizable prompt (PS1)
🏗️ Structure
main.c – Entry point
input.c – Input parsing & signals
functions.c – Command execution
shell.h – Definitions
external.txt – External commands list
⚙️ Build & Run
gcc main.c input.c functions.c -o minishell
./minishell
📚 Concepts Used
fork(), execvp(), waitpid()
Pipes & IPC
Signal handling
Linked list (job control)
👨‍💻 Author

Mathews Roy
