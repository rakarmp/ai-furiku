#!/system/bin/sh
# Optimasi khusus untuk chipset Snapdragon

# Optimasi Adreno GPU
optimize_adreno() {
  # Aktifkan fitur kinerja Adreno
  echo "1" > /sys/class/kgsl/kgsl-3d0/force_bus_on
  echo "1" > /sys/class/kgsl/kgsl-3d0/force_rail_on
  echo "1" > /sys/class/kgsl/kgsl-3d0/force_clk_on
  
  # Atur frekuensi GPU 
  echo "0" > /sys/class/kgsl/kgsl-3d0/devfreq/adrenoboost
  
  # Optimasi GPU governor
  echo "msm-adreno-tz" > /sys/class/kgsl/kgsl-3d0/devfreq/governor
  
  # Aktifkan fitur Elite Gaming jika tersedia
  if [ -f "/sys/module/adreno_idler/parameters/adreno_idler_active" ]; then
    echo "N" > /sys/module/adreno_idler/parameters/adreno_idler_active
  fi
}

# Optimasi Hexagon DSP
optimize_hexagon_dsp() {
  # Aktifkan fitur Hexagon DSP jika tersedia
  if [ -d "/sys/class/devfreq/soc:qcom,cci" ]; then
    echo "performance" > /sys/class/devfreq/soc:qcom,cci/governor
  fi
}

# Panggil fungsi optimasi
optimize_adreno
optimize_hexagon_dsp