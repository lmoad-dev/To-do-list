#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASKS 1000
#define MAX_DESC 256
#define FILENAME "tasks.txt"

#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_MAGENTA "\033[35m"

typedef struct {
    int id;
    int completed;
    char description[MAX_DESC];
} Task;

static Task tasks[MAX_TASKS];
static int task_count = 0;
static int next_id = 1;

void update_next_id(void) {
    int max_id = 0;
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id > max_id) {
            max_id = tasks[i].id;
        }
    }
    next_id = max_id + 1;
}

void load_tasks(void) {
    task_count = 0;
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) return;

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        char *id_str = strtok(line, "|");
        char *comp_str = strtok(NULL, "|");
        char *desc = strtok(NULL, "");

        if (id_str && comp_str && desc && task_count < MAX_TASKS) {
            tasks[task_count].id = atoi(id_str);
            tasks[task_count].completed = atoi(comp_str);
            strncpy(tasks[task_count].description, desc, MAX_DESC - 1);
            tasks[task_count].description[MAX_DESC - 1] = '\0';
            task_count++;
        }
    }
    fclose(fp);
    update_next_id();
}

void save_tasks(void) {
    FILE *fp = fopen(FILENAME, "w");
    if (!fp) {
        printf("%sError: could not save tasks to %s%s\n", COLOR_RED, FILENAME, COLOR_RESET);
        return;
    }
    for (int i = 0; i < task_count; i++) {
        fprintf(fp, "%d|%d|%s\n", tasks[i].id, tasks[i].completed, tasks[i].description);
    }
    fclose(fp);
}

void list_tasks(void) {
    printf("\n%s%s==================== TO-DO LIST ====================%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
    if (task_count == 0) {
        printf("  No tasks found. Your list is empty!\n");
        printf("%s%s====================================================%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
        return;
    }

    printf("%-6s %-12s %s\n", "ID", "Status", "Description");
    printf("----------------------------------------------------\n");
    for (int i = 0; i < task_count; i++) {
        const char *status_str = tasks[i].completed ? "[✓] Done" : "[ ] Pending";
        const char *status_color = tasks[i].completed ? COLOR_GREEN : COLOR_YELLOW;
        printf("%-6d %s%-12s%s %s\n", tasks[i].id, status_color, status_str, COLOR_RESET, tasks[i].description);
    }
    printf("%s%s====================================================%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
}

void add_task(const char *desc) {
    if (!desc || strlen(desc) == 0) {
        printf("%sTask description cannot be empty.%s\n", COLOR_YELLOW, COLOR_RESET);
        return;
    }
    if (task_count >= MAX_TASKS) {
        printf("%sTask list is full.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    tasks[task_count].id = next_id++;
    tasks[task_count].completed = 0;
    strncpy(tasks[task_count].description, desc, MAX_DESC - 1);
    tasks[task_count].description[MAX_DESC - 1] = '\0';
    task_count++;
    save_tasks();
    printf("%sTask #%d added successfully!%s\n", COLOR_GREEN, tasks[task_count - 1].id, COLOR_RESET);
}

void complete_task(int id) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id == id) {
            if (tasks[i].completed) {
                printf("%sTask #%d is already completed.%s\n", COLOR_YELLOW, id, COLOR_RESET);
            } else {
                tasks[i].completed = 1;
                save_tasks();
                printf("%sTask #%d marked as completed!%s\n", COLOR_GREEN, id, COLOR_RESET);
            }
            return;
        }
    }
    printf("%sTask with ID #%d not found.%s\n", COLOR_RED, id, COLOR_RESET);
}

void delete_task(int id) {
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id == id) {
            for (int j = i; j < task_count - 1; j++) {
                tasks[j] = tasks[j + 1];
            }
            task_count--;
            save_tasks();
            printf("%sTask #%d deleted successfully.%s\n", COLOR_GREEN, id, COLOR_RESET);
            return;
        }
    }
    printf("%sTask with ID #%d not found.%s\n", COLOR_RED, id, COLOR_RESET);
}

void clear_all_tasks(void) {
    if (task_count == 0) {
        printf("%sList is already empty.%s\n", COLOR_YELLOW, COLOR_RESET);
        return;
    }
    task_count = 0;
    save_tasks();
    printf("%sAll tasks have been cleared.%s\n", COLOR_GREEN, COLOR_RESET);
}

void show_menu(void) {
    printf("\n%s------ Actions ------%s\n", COLOR_BOLD, COLOR_RESET);
    printf("1. View Tasks\n");
    printf("2. Add Task\n");
    printf("3. Mark Task as Completed\n");
    printf("4. Delete Task\n");
    printf("5. Clear All Tasks\n");
    printf("6. Exit\n");
    printf("%sChoose an option (1-6): %s", COLOR_BOLD, COLOR_RESET);
    fflush(stdout);
}

int main(void) {
    load_tasks();
    printf("%s%sWelcome to Your To-Do List Application!%s\n", COLOR_BOLD, COLOR_MAGENTA, COLOR_RESET);

    char buffer[256];
    while (1) {
        show_menu();
        if (!fgets(buffer, sizeof(buffer), stdin)) break;

        int choice = atoi(buffer);
        if (choice == 1) {
            list_tasks();
        } else if (choice == 2) {
            printf("Enter task description: ");
            fflush(stdout);
            if (fgets(buffer, sizeof(buffer), stdin)) {
                buffer[strcspn(buffer, "\r\n")] = '\0';
                add_task(buffer);
            }
        } else if (choice == 3) {
            printf("Enter task ID to mark as completed: ");
            fflush(stdout);
            if (fgets(buffer, sizeof(buffer), stdin)) {
                complete_task(atoi(buffer));
            }
        } else if (choice == 4) {
            printf("Enter task ID to delete: ");
            fflush(stdout);
            if (fgets(buffer, sizeof(buffer), stdin)) {
                delete_task(atoi(buffer));
            }
        } else if (choice == 5) {
            printf("Are you sure you want to clear all tasks? (y/n): ");
            fflush(stdout);
            if (fgets(buffer, sizeof(buffer), stdin)) {
                if (buffer[0] == 'y' || buffer[0] == 'Y') {
                    clear_all_tasks();
                } else {
                    printf("Operation cancelled.\n");
                }
            }
        } else if (choice == 6) {
            printf("%sGoodbye!%s\n", COLOR_BOLD, COLOR_RESET);
            break;
        } else {
            printf("%sInvalid option. Please choose between 1 and 6.%s\n", COLOR_RED, COLOR_RESET);
        }
    }
    return 0;
}
