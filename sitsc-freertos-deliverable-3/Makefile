# Makefile - builds the SITSC program with every *.c file in this folder against the FreeRTOS Kernel in /opt/FreeRTOS-Kernel

FREERTOS_DIR	?= /opt/FreeRTOS-Kernel
PORT_DIR		:= $(FREERTOS_DIR)/portable/ThirdParty/GCC/Posix

CC			:= gcc
INCLUDES    := -I. -I$(FREERTOS_DIR)/include -I$(PORT_DIR) -I$(PORT_DIR)/utils
CFLAGS		:= -std=gnu11 -O2 -g -Wall -Wextra -D_GNU_SOURCE -pthread $(INCLUDES)
LDFLAGS		:= -pthread

APP_SRC		:= $(wildcard *.c)
APP_HDR		:= $(wildcard *.h)
KERNEL_SRC 	:= $(FREERTOS_DIR)/tasks.c $(FREERTOS_DIR)/queue.c $(FREERTOS_DIR)/timers.c $(FREERTOS_DIR)/list.c $(FREERTOS_DIR)/portable/MemMang/heap_3.c $(PORT_DIR)/port.c $(PORT_DIR)/utils/wait_for_event.c

BUILD		:= build
APP_OBJ		:= $(patsubst %.c, $(BUILD)/app/%.o, $(APP_SRC))
KERNEL_OBJ	:= $(patsubst $(FREERTOS_DIR)/%.c, $(BUILD)/kernel/%.o, $(KERNEL_SRC))

app: $(APP_OBJ) $(KERNEL_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Build the application object files from the .c files in the current directory
$(BUILD)/app/%.o: %.c $(APP_HDR) # Target; prerequisites (Create the directory if it doesn't exist (the directory is the part of the filename before the last /); recipe (Compile the source file into an object file)
	@mkdir -p $(@D) 
	$(CC) $(CFLAGS) -c $< -o $@ 

# Build the kernel object files from the .c files in the FreeRTOS directory (including the subdirectories)
# Also include the FreeRTOSConfig.h file from the current directory
$(BUILD)/kernel/%.o: $(FREERTOS_DIR)/%.c FreeRTOSConfig.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD) app

.PHONY: clean