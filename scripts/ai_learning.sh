#!/system/bin/sh

MODDIR=/data/adb/modules/ai-furiku
CONFIG_DIR=$MODDIR/config
LOG_DIR=/storage/emulated/0/Android/AI-Furiku_logs
LEARNING_DATA=$CONFIG_DIR/learning_data.json
CURRENT_DATE=$(date +%Y%m%d)

# Cek apakah file data sudah ada, jika tidak buat baru
if [ ! -f "$LEARNING_DATA" ]; then
  echo '{"app_usage":{},"battery_stats":[],"performance_profiles":{}}' > $LEARNING_DATA
fi

# Fungsi untuk mengumpulkan data penggunaan aplikasi
collect_app_usage() {
  # Dapatkan 10 aplikasi teratas yang menggunakan CPU
  dumpsys cpuinfo | grep -E "^[0-9]+%" | head -10 | while read line; do
    USAGE=$(echo $line | cut -d "%" -f1)
    APP=$(echo $line | sed 's/^[0-9]*%\s*//g' | cut -d "/" -f1)
    
    if [ ! -z "$APP" ]; then
      # Update data pembelajaran
      CURRENT_JSON=$(cat $LEARNING_DATA)
      NEW_JSON=$(echo $CURRENT_JSON | sed "s/\"app_usage\":{/\"app_usage\":{\"$APP\":$USAGE,/g")
      echo $NEW_JSON > $LEARNING_DATA
    fi
  done
}

# Fungsi untuk mengumpulkan statistik baterai
collect_battery_stats() {
  BATTERY_LEVEL=$(cat /sys/class/power_supply/battery/capacity)
  BATTERY_TEMP=$(cat /sys/class/power_supply/battery/temp)
  BATTERY_TEMP=$(echo "scale=1; $BATTERY_TEMP/10" | bc)
  CHARGING_STATUS=$(cat /sys/class/power_supply/battery/status)
  
  # Update data pembelajaran baterai
  CURRENT_JSON=$(cat $LEARNING_DATA)
  BATTERY_ENTRY="{\"timestamp\":\"$(date +%s)\",\"level\":$BATTERY_LEVEL,\"temp\":$BATTERY_TEMP,\"status\":\"$CHARGING_STATUS\"}"
  NEW_JSON=$(echo $CURRENT_JSON | sed "s/\"battery_stats\":\[/\"battery_stats\":\[$BATTERY_ENTRY,/g")
  echo $NEW_JSON > $LEARNING_DATA
}

# Jalankan pengumpulan data secara periodik
while true; do
  collect_app_usage
  collect_battery_stats
  
  # Tunggu 15 menit sebelum pengumpulan data berikutnya
  sleep 900
done