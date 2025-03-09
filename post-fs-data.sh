#!/system/bin/sh
# Jalankan saat post-fs-data

MODDIR=${0%/*}

# Buat direktori log
mkdir -p /storage/emulated/0/Android/AI-Furiku_logs

# Deteksi jenis chipset
if grep -qE "Snapdragon|Qualcomm" /proc/cpuinfo; then
  echo "snapdragon" > $MODDIR/config/chipset_type
elif grep -qE "MediaTek|MT" /proc/cpuinfo; then
  echo "mediatek" > $MODDIR/config/chipset_type
else
  echo "generic" > $MODDIR/config/chipset_type
fi

# Load konfigurasi default
cp -f $MODDIR/config/default.conf $MODDIR/config/active.conf

# Set permissions
chmod -R 755 $MODDIR/scripts
chmod -R 755 $MODDIR/system/bin