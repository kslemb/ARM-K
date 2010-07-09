PROGRAM			= GettingStarted
SOURCE_DIRS		= . backlight timer0 vic uart syscall generic shell spi i2c enc28j60 lwip_lib
LWIP_INC_DIRS	= lwip/src/include lwip/src/include/ipv4 lwip_lib/ports/lpc_enc28j60/include
INCLUDE_DIRS	= $(SOURCE_DIRS) $(LWIP_INC_DIRS)
WARNING			= -Wall
#WARNING 		+= -Wstrict-prototypes -Wcast-align -Wcast-qual -Wimplicit
LINKER_SCRIPT	= linker.ld

CC      	= arm-elf-gcc
LD      	= arm-elf-gcc
AR      	= arm-elf-ar
AS      	= arm-elf-as
ODUMP	  	= arm-elf-objdump
OBJCOPY		= arm-elf-objcopy

CFLAGS  	= -c -fno-common -mcpu=arm7tdmi-s $(WARNING)
AFLAGS  	= -ahls=$(<:.s=.lst) -mapcs-32
ODFLAGS 	= -h -S

# Linker flags
# -Wl,...:	tell GCC to pass this to linker.
# -Map:		create map file
# --cref:	add cross reference to  map file
# -lc:		add libc (newlib) library
# -lgcc:	add libgcc library
# -T:		set the linker script file
LDFLAGS		= -nostartfiles -Wl,-Map=$(TARGET_DIR)/$(PROGRAM).map,--cref
LDFLAGS		+= -T$(LINKER_SCRIPT)

SEARCH_PATH_C	:= $(addsuffix /*.c,$(SOURCE_DIRS))
SEARCH_PATH_ASM	:= $(addsuffix /*.s,$(SOURCE_DIRS))
CDEP		:= $(notdir $(patsubst %.c, %.o, $(wildcard $(SEARCH_PATH_C))))
SDEP		:= $(notdir $(patsubst %.s, %.o, $(wildcard $(SEARCH_PATH_ASM))))

# Get arguments from command line
arg:=$(MAKECMDGOALS)

# Process release and debug options
ifeq (Release,$(filter Release,$(arg)))
# Set Target directory to Release
 TARGET_DIR	= Release
# Create directory if is not created early
 $(shell mkdir -p $(TARGET_DIR))
 CFLAGS+= -O2
 CFLAGS+= -fomit-frame-pointer
 LDFLAGS+= -lc -lgcc
else
 ifeq (Debug,$(filter Debug,$(arg)))
 # Set Target directory to Debug
#	@echo "Making Debug target..."
  TARGET_DIR = Debug
 # Create directory if is not created early
  $(shell mkdir -p $(TARGET_DIR))
  CFLAGS+= -O0
  CFLAGS+= -g
  LDFLAGS+= -lg -lgcc
else
 # For default use Release directory
 TARGET_DIR	= Release
 # Create directory if is not created early
 $(shell mkdir -p $(TARGET_DIR))
 CFLAGS+= -O2
 LDFLAGS+= -lc -lgcc
endif
endif

# Targets:

Release:	all

Debug:		all

all:	$(TARGET_DIR)/$(PROGRAM).hex

$(TARGET_DIR)/$(PROGRAM).hex: $(TARGET_DIR)/$(PROGRAM).elf Makefile
	@echo "Creating firmware" $(PROGRAM)".hex"
	$(OBJCOPY) -O ihex $(TARGET_DIR)/$(PROGRAM).elf $(TARGET_DIR)/$(PROGRAM).hex
	@echo "Create extended listing" $(PROGRAM)".dmp"
	$(ODUMP) $(ODFLAGS) $(TARGET_DIR)/$(PROGRAM).elf > $(TARGET_DIR)/$(PROGRAM).dmp

$(TARGET_DIR)/$(PROGRAM).elf: $(CDEP) $(SDEP)
	@echo "Linking" $(PROGRAM)
	$(LD) $^ $(LDFLAGS) -o $(TARGET_DIR)/$(PROGRAM).elf

VPATH = $(SOURCE_DIRS)
vpath = %.h $(INCLUDE_DIRS)

%.o: %.s Makefile
	@echo "Assembling" $<
	$(AS) $(AFLAGS) $< -o $@

%.o: %.c Makefile
	@echo "Compilling" $<
#	$(CC) $(CFLAGS) -MD $<
	$(CC) $(CFLAGS) -MD $(addprefix -I,$(INCLUDE_DIRS)) $<


include $(wildcard *.d)

clean:
	@echo "Clean project..."
	rm -rf *.o *.lst *.d
	rm -rf Release/*.*
	rm -rf Debug/*.*

Program:
#  For LPC2148 with 12 MHz crystal
	lpc21isp\lpc21isp -control Release/$(PROGRAM).hex com1 38400 12000

.PHONY: Release Debug all Program clean
