/**
 * AI-Furiku Daemon
 * Program utama untuk daemon AI-Furiku
 */

 #include "ai_furiku.h"
 #include <signal.h>
 #include <errno.h>
 #include <pthread.h>
 
 // Flag untuk menandakan daemon berjalan
 volatile int running = 1;
 
 // Thread untuk memproses state device
 void *device_state_thread(void *arg) {
     DeviceState state;
     char log_message[MAX_BUFFER_SIZE];
     
     // Deteksi hardware
     detect_device_hardware(&state);
     
     // Loop terus selama daemon running
     while (running) {
         // Update device state
         update_device_state(&state);
         
         // Log current state
         snprintf(log_message, MAX_BUFFER_SIZE, 
                  "Device state: Battery: %d%%, Temp: %.1f°C, CPU: %d%%, Memory: %d%%, "
                  "Screen: %s, Charging: %s, Gaming: %s",
                  state.battery_level, state.battery_temp,
                  state.cpu_usage, state.memory_usage,
                  state.screen_state ? "ON" : "OFF",
                  state.charging_mode == CHARGING_NONE ? "No" : 
                  (state.charging_mode == CHARGING_FAST ? "Fast" : "Slow"),
                  state.is_gaming ? "Yes" : "No");
         write_log(log_message);
         
         // Read operation mode from config
         char op_mode[32] = "auto";
         read_config_value("operation_mode", op_mode, sizeof(op_mode));
         
         // Determine operation mode
         OperationMode mode;
         if (strcmp(op_mode, "auto") == 0) {
             // AI decides optimal mode
             mode = determine_optimal_mode(&state);
             
             snprintf(log_message, MAX_BUFFER_SIZE, 
                      "AI selected mode: %s",
                      mode == MODE_PERFORMANCE ? "Performance" :
                      mode == MODE_BALANCED ? "Balanced" : "Battery");
             write_log(log_message);
         } 
         else if (strcmp(op_mode, "performance") == 0) {
             mode = MODE_PERFORMANCE;
         } 
         else if (strcmp(op_mode, "battery") == 0) {
             mode = MODE_BATTERY;
         } 
         else {
             // Default to balanced
             mode = MODE_BALANCED;
         }
         
         // Apply kernel parameters for selected mode
         apply_kernel_parameters(mode, &state);
         
         // Optimize charging if device is charging
         if (state.charging_mode != CHARGING_NONE) {
             optimize_charging(&state);
         }
         
         // Manage thermal state
         manage_thermal_state(&state);
         
         // Sleep for 30 seconds before next cycle
         sleep(30);
     }
     
     return NULL;
 }
 
 // Thread untuk mengumpulkan data usage
 void *usage_data_thread(void *arg) {
     AppUsage apps[10];
     char data_file[MAX_PATH_LENGTH];
     char log_message[MAX_BUFFER_SIZE];
     FILE *fp;
     
     // Format data file path
     snprintf(data_file, MAX_PATH_LENGTH, "%s/usage_data.json", CONFIG_DIR);
     
     // Loop terus selama daemon running
     while (running) {
         // Collect app usage data
         collect_app_usage_data(apps, 10);
         
         // Open data file in append mode
         fp = fopen(data_file, "a");
         if (fp) {
             // Get current timestamp
             time_t now;
             time(&now);
             
             // Write timestamp entry
             fprintf(fp, "{\"timestamp\":%ld,\"apps\":[", (long)now);
             
             // Write app usage data
             for (int i = 0; i < 10 && apps[i].cpu_usage > 0; i++) {
                 if (i > 0) {
                     fprintf(fp, ",");
                 }
                 fprintf(fp, "{\"name\":\"%s\",\"cpu\":%d}", 
                         apps[i].name, apps[i].cpu_usage);
             }
             
             // Close entry
             fprintf(fp, "]}\n");
             fclose(fp);
         }
         
         // Analyze usage patterns every 6 hours
         static int counter = 0;
         if (counter >= 720) { // 30s * 720 = 6 hours
             analyze_usage_patterns(data_file);
             counter = 0;
         }
         counter++;
         
         // Sleep for 5 minutes before next data collection
         sleep(300);
     }
     
     return NULL;
 }
 
 // Handler untuk sinyal
 void signal_handler(int sig) {
     write_log("Received signal to terminate");
     running = 0;
 }
 
 // Fungsi main
 int main(int argc, char *argv[]) {
     pthread_t state_thread, data_thread;
     struct sigaction sa;
     
     // Inisialisasi
     if (initialize_ai_furiku() != 0) {
         write_log("Failed to initialize AI-Furiku");
         return 1;
     }
     
     // Setup signal handler
     memset(&sa, 0, sizeof(sa));
     sa.sa_handler = signal_handler;
     sigaction(SIGTERM, &sa, NULL);
     sigaction(SIGINT, &sa, NULL);
     
     write_log("AI-Furiku daemon starting");
     
     // Buat thread untuk device state processing
     if (pthread_create(&state_thread, NULL, device_state_thread, NULL) != 0) {
         write_log("Failed to create device state thread");
         return 1;
     }
     
     // Buat thread untuk data usage collection
     if (pthread_create(&data_thread, NULL, usage_data_thread, NULL) != 0) {
         write_log("Failed to create usage data thread");
         running = 0;
         pthread_join(state_thread, NULL);
         return 1;
     }
     
     // Log daemon started
     write_log("AI-Furiku daemon running");
     
     // Tunggu thread selesai
     pthread_join(state_thread, NULL);
     pthread_join(data_thread, NULL);
     
     // Log daemon stopped
     write_log("AI-Furiku daemon stopped");
     
     return 0;
 }