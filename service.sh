#!/system/bin/sh
# Mulai layanan AI-Furiku setelah boot selesai

MODDIR=${0%/*}

# Tunggu hingga sistem benar-benar dimulai
until [ "$(getprop sys.boot_completed)" = "1" ]; do
  sleep 1
done

# Path ke logs
LOG_DIR=/storage/emulated/0/Android/AI-Furiku_logs
mkdir -p $LOG_DIR

# Mulai service utama
$MODDIR/scripts/ai_furiku_service.sh > $LOG_DIR/service_$(date +%Y%m%d_%H%M%S).log 2>&1 &