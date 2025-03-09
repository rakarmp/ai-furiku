#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CONFIG_PATH "/data/adb/modules/ai-furiku/config/active.conf"
#define DAEMON_NAME "ai_furiku_daemon"

// Function prototypes
void display_status();
void display_menu();
int get_user_choice();
void change_operation_mode();
void toggle_thermal_throttling();
void restart_daemon();
int check_root();
int is_daemon_running();
void read_battery_info();
void read_cpu_info();
void read_current_settings();

// Global variables to store current settings
char current_mode[20] = "auto";
int thermal_throttling = 1;

int main() {
    if (!check_root()) {
        printf("Error: Root access required.\n");
        printf("Please run with: su -c furiku\n");
        return 1;
    }

    int choice;
    int exit_flag = 0;

    while (!exit_flag) {
        system("clear");
        printf("╔════════════════════════════════════╗\n");
        printf("║          AI-FURIKU MANAGER         ║\n");
        printf("╚════════════════════════════════════╝\n\n");

        display_status();
        display_menu();
        
        choice = get_user_choice();
        
        switch (choice) {
            case 1:
                change_operation_mode();
                break;
            case 2:
                toggle_thermal_throttling();
                break;
            case 3:
                restart_daemon();
                break;
            case 0:
                exit_flag = 1;
                break;
            default:
                printf("Invalid choice. Press Enter to continue...");
                getchar();
                getchar();
                break;
        }
    }
    
    return 0;
}

void display_status() {
    printf("📊 SYSTEM STATUS:\n");
    printf("==================\n");
    
    if (is_daemon_running()) {
        printf("Module Status: ✅ Running\n");
    } else {
        printf("Module Status: ❌ Not Running\n");
    }
    
    read_battery_info();
    read_cpu_info();
    read_current_settings();
    
    printf("\n");
}

void display_menu() {
    printf("📋 MENU OPTIONS:\n");
    printf("=================\n");
    printf("1. Change Operation Mode (Current: %s)\n", current_mode);
    printf("2. Toggle Thermal Throttling (Current: %s)\n", thermal_throttling ? "Enabled" : "Disabled");
    printf("3. Restart Daemon\n");
    printf("0. Exit\n\n");
}

int get_user_choice() {
    int choice;
    printf("Enter your choice: ");
    scanf("%d", &choice);
    return choice;
}

void change_operation_mode() {
    system("clear");
    printf("╔════════════════════════════════════╗\n");
    printf("║       CHANGE OPERATION MODE        ║\n");
    printf("╚════════════════════════════════════╝\n\n");
    
    printf("Current mode: %s\n\n", current_mode);
    printf("Available modes:\n");
    printf("1. Auto (Adaptive)\n");
    printf("2. Performance\n");
    printf("3. Balanced\n");
    printf("4. Battery Saver\n");
    printf("0. Cancel\n\n");
    
    int choice;
    printf("Select mode: ");
    scanf("%d", &choice);
    
    char new_mode[20];
    int changed = 1;
    
    switch (choice) {
        case 1:
            strcpy(new_mode, "auto");
            break;
        case 2:
            strcpy(new_mode, "performance");
            break;
        case 3:
            strcpy(new_mode, "balanced");
            break;
        case 4:
            strcpy(new_mode, "battery");
            break;
        case 0:
            changed = 0;
            break;
        default:
            printf("Invalid choice. Operation mode not changed.\n");
            changed = 0;
            printf("Press Enter to continue...");
            getchar();
            getchar();
            return;
    }
    
    if (changed) {
        // Read the existing config
        FILE *config_file = fopen(CONFIG_PATH, "r");
        if (!config_file) {
            printf("Error: Cannot read configuration file.\n");
            printf("Press Enter to continue...");
            getchar();
            getchar();
            return;
        }
        
        // Create a temporary file
        char temp_path[256];
        sprintf(temp_path, "%s.tmp", CONFIG_PATH);
        FILE *temp_file = fopen(temp_path, "w");
        if (!temp_file) {
            printf("Error: Cannot create temporary file.\n");
            fclose(config_file);
            printf("Press Enter to continue...");
            getchar();
            getchar();
            return;
        }
        
        // Copy the contents, replacing the mode line
        char line[256];
        while (fgets(line, sizeof(line), config_file)) {
            if (strncmp(line, "operation_mode=", 15) == 0) {
                fprintf(temp_file, "operation_mode=%s\n", new_mode);
            } else {
                fprintf(temp_file, "%s", line);
            }
        }
        
        fclose(config_file);
        fclose(temp_file);
        
        // Replace the old file with the new one
        rename(temp_path, CONFIG_PATH);
        
        printf("Operation mode changed to: %s\n", new_mode);
        strcpy(current_mode, new_mode);
        
        // Ask if user wants to restart the daemon
        printf("Do you want to restart the daemon to apply changes? (y/n): ");
        char answer;
        scanf(" %c", &answer);
        
        if (answer == 'y' || answer == 'Y') {
            restart_daemon();
        }
    }
    
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

void toggle_thermal_throttling() {
    // Toggle thermal throttling value
    thermal_throttling = !thermal_throttling;
    
    // Read the existing config
    FILE *config_file = fopen(CONFIG_PATH, "r");
    if (!config_file) {
        printf("Error: Cannot read configuration file.\n");
        printf("Press Enter to continue...");
        getchar();
        getchar();
        return;
    }
    
    // Create a temporary file
    char temp_path[256];
    sprintf(temp_path, "%s.tmp", CONFIG_PATH);
    FILE *temp_file = fopen(temp_path, "w");
    if (!temp_file) {
        printf("Error: Cannot create temporary file.\n");
        fclose(config_file);
        printf("Press Enter to continue...");
        getchar();
        getchar();
        return;
    }
    
    // Copy the contents, replacing the thermal line
    char line[256];
    while (fgets(line, sizeof(line), config_file)) {
        if (strncmp(line, "thermal_throttling_auto=", 24) == 0) {
            fprintf(temp_file, "thermal_throttling_auto=%d\n", thermal_throttling);
        } else {
            fprintf(temp_file, "%s", line);
        }
    }
    
    fclose(config_file);
    fclose(temp_file);
    
    // Replace the old file with the new one
    rename(temp_path, CONFIG_PATH);
    
    printf("Thermal throttling %s\n", thermal_throttling ? "enabled" : "disabled");
    
    // Ask if user wants to restart the daemon
    printf("Do you want to restart the daemon to apply changes? (y/n): ");
    char answer;
    scanf(" %c", &answer);
    
    if (answer == 'y' || answer == 'Y') {
        restart_daemon();
    }
    
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

void restart_daemon() {
    printf("Restarting AI-Furiku daemon...\n");
    
    // Kill the current daemon
    system("killall -9 ai_furiku_daemon");
    
    // Wait a moment
    usleep(500000);
    
    // Start the daemon again
    system("ai_furiku_daemon &");
    
    // Wait a moment
    usleep(500000);
    
    if (is_daemon_running()) {
        printf("Daemon successfully restarted.\n");
    } else {
        printf("Warning: Daemon may not have started correctly.\n");
    }
    
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

int check_root() {
    return (geteuid() == 0);
}

int is_daemon_running() {
    FILE *fp;
    char path[1035];
    int running = 0;
    
    // Open the command for reading
    fp = popen("ps | grep ai_furiku_daemon | grep -v grep", "r");
    if (fp == NULL) {
        return 0;
    }
    
    // Read the output a line at a time
    if (fgets(path, sizeof(path), fp) != NULL) {
        running = 1;
    }
    
    // Close the pipe
    pclose(fp);
    
    return running;
}

void read_battery_info() {
    int capacity = -1;
    float temp = -1.0;
    char status[64] = "Unknown";
    
    // Read battery capacity
    FILE *cap_file = fopen("/sys/class/power_supply/battery/capacity", "r");
    if (cap_file) {
        fscanf(cap_file, "%d", &capacity);
        fclose(cap_file);
    }
    
    // Read battery temperature
    FILE *temp_file = fopen("/sys/class/power_supply/battery/temp", "r");
    if (temp_file) {
        int temp_raw;
        fscanf(temp_file, "%d", &temp_raw);
        temp = temp_raw / 10.0;
        fclose(temp_file);
    }
    
    // Read charging status
    FILE *status_file = fopen("/sys/class/power_supply/battery/status", "r");
    if (status_file) {
        fscanf(status_file, "%s", status);
        fclose(status_file);
    }
    
    printf("Battery: %d%% - %.1f°C - %s\n", capacity, temp, status);
}

void read_cpu_info() {
    FILE *fp;
    char result[128];
    
    // Open the command for reading CPU usage
    fp = popen("top -n 1 | grep '%cpu' | awk '{print $2}'", "r");
    if (fp == NULL) {
        printf("CPU Usage: Unable to read\n");
        return;
    }
    
    // Read the output
    if (fgets(result, sizeof(result), fp) != NULL) {
        // Remove newline
        result[strcspn(result, "\n")] = 0;
        printf("CPU Usage: %s%%\n", result);
    } else {
        printf("CPU Usage: Unable to read\n");
    }
    
    // Close the pipe
    pclose(fp);
}

void read_current_settings() {
    FILE *config_file = fopen(CONFIG_PATH, "r");
    if (!config_file) {
        printf("Current Settings: Unable to read\n");
        return;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), config_file)) {
        if (strncmp(line, "operation_mode=", 15) == 0) {
            strcpy(current_mode, line + 15);
            // Remove newline
            current_mode[strcspn(current_mode, "\n")] = 0;
        } else if (strncmp(line, "thermal_throttling_auto=", 24) == 0) {
            thermal_throttling = atoi(line + 24);
        }
    }
    
    fclose(config_file);
    
    printf("Current Settings: %s mode, Thermal: %s\n", 
        current_mode, 
        thermal_throttling ? "Enabled" : "Disabled");
}