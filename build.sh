#!/bin/sh
# Buat direktori
mkdir -p obj
mkdir -p system/bin

# Periksa apakah direktori berhasil dibuat
echo "Direktori berhasil dibuat:"
ls -la | grep obj
ls -la | grep system

# Kompilasi file sumber
echo "Mengkompilasi file sumber..."
clang -Wall -Wextra -O2 -fPIC -c common/ai_furiku.c -o obj/ai_furiku.o
clang -Wall -Wextra -O2 -fPIC -c common/ai_furiku_daemon.c -o obj/ai_furiku_daemon.o
clang -Wall -Wextra -O2 -fPIC -c common/furiku.c -o obj/furiku.o

# Link executable
echo "Linking executable..."
clang obj/ai_furiku.o obj/ai_furiku_daemon.o -o system/bin/ai_furiku_daemon
clang obj/furiku.o -o system/bin/furiku

# Mencoba strip jika ada
strip system/bin/ai_furiku_daemon 2>/dev/null || echo "Strip tidak tersedia untuk ai_furiku_daemon"
strip system/bin/furiku 2>/dev/null || echo "Strip tidak tersedia untuk furiku"

echo "Kompilasi selesai. Output:"
ls -la system/bin/