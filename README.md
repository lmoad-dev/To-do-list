# CLI To-Do List Application in C++

A lightweight command-line To-Do List manager with color-coded interface and automatic file persistence.

## Features
- **View Tasks**: Displays pending and completed tasks in a formatted table.
- **Add Tasks**: Add new tasks with descriptions.
- **Mark Completed**: Mark any task as done.
- **Delete Tasks**: Remove specific tasks by ID.
- **Clear All**: Reset and delete all tasks.
- **Data Persistence**: Tasks are saved automatically to `tasks.txt`.

## Build & Run

### Prerequisites
Make sure `g++` (or `gcc-c++`) is installed:
```bash
sudo dnf install -y gcc-c++
```

### Compile
```bash
make
```

### Run
```bash
./todo_app
```
or run with make:
```bash
make run
```
