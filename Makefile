# Makefile untuk AI-Furiku (versi Termux)

# Compiler dan tools
CC = clang
STRIP = strip

# Flags - catatan: tidak perlu -static karena Termux tidak sepenuhnya mendukungnya
CFLAGS += -Wall -Wextra -O2 -fPIC
LDFLAGS += 

# Direktori
SRC_DIR = common
OBJ_DIR = obj
BIN_DIR = system/bin

# File sumber
SRC = $(SRC_DIR)/ai_furiku.c $(SRC_DIR)/ai_furiku_daemon.c $(SRC_DIR)/furiku.c

# File objek
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# Target binari
DAEMON = $(BIN_DIR)/ai_furiku_daemon
FURIKU = $(BIN_DIR)/furiku

# Target utama
all: prepare $(DAEMON) $(FURIKU)

prepare:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

# Aturan kompilasi untuk file objek
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Aturan untuk ai_furiku_daemon
$(DAEMON): $(OBJ_DIR)/ai_furiku.o $(OBJ_DIR)/ai_furiku_daemon.o
	$(CC) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

# Aturan untuk furiku
$(FURIKU): $(OBJ_DIR)/furiku.o
	$(CC) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(DAEMON)
	rm -f $(FURIKU)

.PHONY: all prepare clean