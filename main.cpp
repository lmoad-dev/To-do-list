#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cctype>

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
}

static inline std::string trim(const std::string& str) {
    const auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    const auto end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

struct Task {
    int id;
    std::string description;
    bool completed;
};

class TodoList {
private:
    std::vector<Task> tasks;
    std::string filename;
    int nextId;

    void updateNextId() {
        int maxId = 0;
        for (const auto& task : tasks) {
            if (task.id > maxId) {
                maxId = task.id;
            }
        }
        nextId = maxId + 1;
    }

public:
    explicit TodoList(const std::string& file) : filename(file), nextId(1) {
        loadFromFile();
    }

    void loadFromFile() {
        tasks.clear();
        std::ifstream file(filename);
        if (!file.is_open()) {
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string idStr, completedStr, description;

            if (std::getline(ss, idStr, '|') &&
                std::getline(ss, completedStr, '|') &&
                std::getline(ss, description)) {
                try {
                    int parsedId = std::stoi(trim(idStr));
                    if (parsedId > 0 && !trim(description).empty()) {
                        Task task;
                        task.id = parsedId;
                        task.completed = (trim(completedStr) == "1");
                        task.description = description;
                        tasks.push_back(task);
                    }
                } catch (...) {
                    // Ignore malformed line
                }
            }
        }
        updateNextId();
    }

    void saveToFile() const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << Color::RED << "Error: Unable to save tasks to " << filename << Color::RESET << "\n";
            return;
        }

        for (const auto& task : tasks) {
            file << task.id << "|" << (task.completed ? "1" : "0") << "|" << task.description << "\n";
        }
    }

    void addTask(const std::string& rawDescription) {
        std::string description = trim(rawDescription);
        if (description.empty()) {
            std::cout << Color::YELLOW << "Task description cannot be empty.\n" << Color::RESET;
            return;
        }

        // Replace any pipe characters or newlines to preserve storage format integrity
        for (char& c : description) {
            if (c == '|' || c == '\n' || c == '\r') {
                c = ' ';
            }
        }
        description = trim(description);
        if (description.empty()) {
            std::cout << Color::YELLOW << "Task description cannot be empty.\n" << Color::RESET;
            return;
        }

        Task task;
        task.id = nextId++;
        task.description = description;
        task.completed = false;
        tasks.push_back(task);
        saveToFile();
        std::cout << Color::GREEN << "Task #" << task.id << " added successfully!\n" << Color::RESET;
    }

    void listTasks() const {
        std::cout << "\n" << Color::BOLD << Color::CYAN << "==================== TO-DO LIST ====================" << Color::RESET << "\n";
        if (tasks.empty()) {
            std::cout << "  No tasks found. Your list is empty!\n";
            std::cout << Color::CYAN << "====================================================\n" << Color::RESET;
            return;
        }

        std::cout << std::left 
                  << std::setw(6)  << "ID"
                  << std::setw(14) << "Status"
                  << "Description" << "\n";
        std::cout << "----------------------------------------------------\n";

        for (const auto& task : tasks) {
            std::cout << std::left << std::setw(6) << task.id;
            if (task.completed) {
                // "[✓] Done" is 8 display columns wide; pad with 6 spaces to reach 14 columns
                std::cout << Color::GREEN << "[✓] Done" << Color::RESET << "      ";
            } else {
                // "[ ] Pending" is 11 display columns wide; pad with 3 spaces to reach 14 columns
                std::cout << Color::YELLOW << "[ ] Pending" << Color::RESET << "   ";
            }
            std::cout << task.description << "\n";
        }
        std::cout << Color::CYAN << "====================================================\n" << Color::RESET;
    }

    void completeTask(int id) {
        for (auto& task : tasks) {
            if (task.id == id) {
                if (task.completed) {
                    std::cout << Color::YELLOW << "Task #" << id << " is already marked as completed.\n" << Color::RESET;
                } else {
                    task.completed = true;
                    saveToFile();
                    std::cout << Color::GREEN << "Task #" << id << " marked as completed!\n" << Color::RESET;
                }
                return;
            }
        }
        std::cout << Color::RED << "Task with ID #" << id << " not found.\n" << Color::RESET;
    }

    void deleteTask(int id) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if (it->id == id) {
                tasks.erase(it);
                saveToFile();
                std::cout << Color::GREEN << "Task #" << id << " deleted successfully.\n" << Color::RESET;
                return;
            }
        }
        std::cout << Color::RED << "Task with ID #" << id << " not found.\n" << Color::RESET;
    }

    void clearAllTasks() {
        if (tasks.empty()) {
            std::cout << Color::YELLOW << "List is already empty.\n" << Color::RESET;
            return;
        }
        tasks.clear();
        nextId = 1;
        saveToFile();
        std::cout << Color::GREEN << "All tasks have been cleared.\n" << Color::RESET;
    }

    bool empty() const {
        return tasks.empty();
    }
};

void showMenu() {
    std::cout << "\n" << Color::BOLD << "------ Actions ------" << Color::RESET << "\n";
    std::cout << "1. View Tasks\n";
    std::cout << "2. Add Task\n";
    std::cout << "3. Mark Task as Completed\n";
    std::cout << "4. Delete Task\n";
    std::cout << "5. Clear All Tasks\n";
    std::cout << "6. Exit\n";
    std::cout << Color::BOLD << "Choose an option (1-6): " << Color::RESET;
    std::cout.flush();
}

int main() {
    TodoList todo("tasks.txt");
    std::cout << Color::BOLD << Color::MAGENTA << "Welcome to Your C++ To-Do List Application!" << Color::RESET << "\n";

    std::string line;
    while (true) {
        showMenu();
        if (!std::getline(std::cin, line)) {
            std::cout << "\n" << Color::BOLD << "Goodbye!\n" << Color::RESET;
            break;
        }

        std::string choice = trim(line);
        if (choice == "1") {
            todo.listTasks();
        } else if (choice == "2") {
            std::cout << "Enter task description: ";
            std::string desc;
            if (!std::getline(std::cin, desc)) {
                std::cout << "\n" << Color::BOLD << "Goodbye!\n" << Color::RESET;
                break;
            }
            todo.addTask(desc);
        } else if (choice == "3") {
            std::cout << "Enter task ID to mark as completed: ";
            std::string idStr;
            if (!std::getline(std::cin, idStr)) {
                std::cout << "\n" << Color::BOLD << "Goodbye!\n" << Color::RESET;
                break;
            }
            try {
                size_t pos = 0;
                std::string trimmedId = trim(idStr);
                int id = std::stoi(trimmedId, &pos);
                if (pos != trimmedId.length() || id <= 0) {
                    std::cout << Color::RED << "Invalid ID. Please enter a valid positive number.\n" << Color::RESET;
                } else {
                    todo.completeTask(id);
                }
            } catch (...) {
                std::cout << Color::RED << "Invalid ID. Please enter a valid positive number.\n" << Color::RESET;
            }
        } else if (choice == "4") {
            std::cout << "Enter task ID to delete: ";
            std::string idStr;
            if (!std::getline(std::cin, idStr)) {
                std::cout << "\n" << Color::BOLD << "Goodbye!\n" << Color::RESET;
                break;
            }
            try {
                size_t pos = 0;
                std::string trimmedId = trim(idStr);
                int id = std::stoi(trimmedId, &pos);
                if (pos != trimmedId.length() || id <= 0) {
                    std::cout << Color::RED << "Invalid ID. Please enter a valid positive number.\n" << Color::RESET;
                } else {
                    todo.deleteTask(id);
                }
            } catch (...) {
                std::cout << Color::RED << "Invalid ID. Please enter a valid positive number.\n" << Color::RESET;
            }
        } else if (choice == "5") {
            if (todo.empty()) {
                std::cout << Color::YELLOW << "List is already empty.\n" << Color::RESET;
                continue;
            }
            std::cout << "Are you sure you want to clear all tasks? (y/n): ";
            std::string confirm;
            if (!std::getline(std::cin, confirm)) {
                std::cout << "\n" << Color::BOLD << "Goodbye!\n" << Color::RESET;
                break;
            }
            confirm = trim(confirm);
            if (confirm == "y" || confirm == "Y") {
                todo.clearAllTasks();
            } else {
                std::cout << "Operation cancelled.\n";
            }
        } else if (choice == "6") {
            std::cout << Color::BOLD << "Goodbye!\n" << Color::RESET;
            break;
        } else {
            std::cout << Color::RED << "Invalid option. Please choose between 1 and 6.\n" << Color::RESET;
        }
    }

    return 0;
}
