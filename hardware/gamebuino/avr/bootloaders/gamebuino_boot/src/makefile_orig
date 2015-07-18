# location of the arduino install folder
ARDUINO_DIR = C:/arduino-1.0.5

# name of this project (used for the output files)
TARGET = gamebuino_boot

# target folder location
OUTPUT_DIR = obj

# compiler flags for c files
CFLAGS = -c -g -Os -Wall -fno-inline-small-functions -fno-split-wide-types -mshort-calls -mmcu=atmega328p -DF_CPU=16000000L  '-DLED_START_FLASHES=3' '-DBAUD_RATE=115200' \
			'-DMMC_CS=PORTB2' '-DMMC_PORT=PORTB' '-DMMC_DDR=DDRB'
#			'-DMMC_CS=PORTD4' '-DMMC_PORT=PORTD' '-DMMC_DDR=DDRD'




# linker flags
LINK_FLAGS =	-g -Wall -Os -fno-inline-small-functions -fno-split-wide-types -mshort-calls -mmcu=atmega328p -Wl,--relax -Wl,--gc-sections -nostartfiles -nostdlib \
				-Wl,--section-start=.text=0x7800			\
				-Wl,--section-start=.jumps=0x7ffa			\
				-Wl,--section-start=.version=0x7ffe			\
				-Wl,--undefined=optiboot_version			\
				-Wl,--undefined=_jumptable


# location of various tools
BIN = $(ARDUINO_DIR)/hardware/tools/avr/bin
VAR_DIR = $(ARDUINO_DIR)/hardware/arduino/variants/standard
GCC = $(BIN)/avr-gcc
COPY = $(BIN)/avr-objcopy
DUMP = $(BIN)/avr-objdump

# set up all the include folders
INCLUDES =	-I$(VAR_DIR)									\
			$(foreach inc,., -I$(inc)/)

# create a list of all the obj files to create for the project folders
PROJECT_OBJS = $(foreach folder, ., $(addprefix $(OUTPUT_DIR)/$(folder)/, $(addsuffix .o, $(notdir $(wildcard $(folder)/*.cpp) $(wildcard $(folder)/*.c)))))

# create a list of dependency files (one for each project obj file)
DEPENDENCIES = $(PROJECT_OBJS:.o=.d)

# main build target
.PHONY: all
all: $(OUTPUT_DIR)/$(TARGET).hex $(OUTPUT_DIR)/$(TARGET).lst
	@echo Finished building $<
	@echo
	
# rule to build the hex file
$(OUTPUT_DIR)/$(TARGET).hex: $(OUTPUT_DIR)/$(TARGET).elf
	@mkdir -p $(dir $@)
	@echo "Copying $@"
	@$(COPY) -j .text -j .jumps -j .version --set-section-flags .version=alloc,load -O ihex $< $@

	#.version=alloc,load

# rule to build the elf file
$(OUTPUT_DIR)/$(TARGET).elf: $(PROJECT_OBJS)
	@echo "Linking $@"
	@$(GCC) $(LINK_FLAGS) -o $(OUTPUT_DIR)/$(TARGET).elf $(PROJECT_OBJS)

# rule to build the lst file
$(OUTPUT_DIR)/$(TARGET).lst: $(OUTPUT_DIR)/$(TARGET).elf
	@echo "Dumping $@"
	@$(DUMP) -h -S $< > $@

#include the dependencies, if they exist	
-include $(DEPENDENCIES)

# rule to build the project .c files
$(OUTPUT_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(GCC) $(CFLAGS) -I$(VAR_DIR) $(INCLUDES) -MMD $< -o $@

clean:
	@echo Cleaning...
	@-rm -f $(OUTPUT_DIR)/$(TARGET).eep
	@-rm -f $(OUTPUT_DIR)/$(TARGET).hex
	@-rm -f $(OUTPUT_DIR)/$(TARGET).elf
	@-rm -f $(PROJECT_OBJS) $(DEPENDENCIES)

rebuild: clean all
