# 🐚 MiniShell – Custom Linux Shell in C

## 📌 Overview
MiniShell is a simple Unix-like shell implemented in C. It supports command execution, piping, and job control, demonstrating core system programming concepts.

---

## 🚀 Features
- Built-in commands: `cd`, `pwd`, `echo`, `exit`, `jobs`, `fg`, `bg`
- Executes external commands using `execvp()`
- Supports piping (`|`) between multiple commands
- Job control (foreground & background processes)
- Signal handling (`Ctrl+C`, `Ctrl+Z`)
- Custom shell prompt (`PS1` support)

---

## 🏗️ Project Structure
- `main.c` – Shell entry point
- `input.c` – Input handling & parsing
- `functions.c` – Command execution logic
- `shell.h` – Definitions & declarations
- `external.txt` – External commands list

---

## ⚙️ Compilation & Run
```bash
gcc main.c input.c functions.c -o minishell
./minishell
Mathews Roy
