MAKE_DIR = $(notdir $(shell pwd))

#  Project Settings
PROJECT=BragMap_Touch
ISYSTEM_INC_DIR = /usr/arm-none-eabi/include
LINKER_SCRIPT = Drivers/STM32F0_Discovery/stm32f0xx_flash.ld

# list all include directories here
INCLUDE_DIRS = Drivers/RS232_Driver/ Drivers/RS232_Driver/Hardware/
INCLUDE_DIRS += Drivers/DAC_Driver/ Drivers/DAC_Driver/Hardware/
INCLUDE_DIRS += Drivers/GPIO_Touch_Driver/ Drivers/GPIO_Touch_Driver/Hardware/
INCLUDE_DIRS += Drivers/SYSTIME_Driver/ Drivers/SYSTIME_Driver/Hardware/
INCLUDE_DIRS += Drivers/STM32F0_Discovery/ Drivers/STM32F0_Discovery/Hardware/
INCLUDE_DIRS += Drivers/CTS_NVM_Driver/ Drivers/CTS_NVM_Driver/Hardware/ 
INCLUDE_DIRS += Drivers/STM32F0_Discovery/std_per_drv/inc/ Drivers/STM32F0_Discovery/cmsis/
INCLUDE_DIRS += Libs/TERM_Lib/ ./ Drivers/ Libs/
INCLUDE_DIRS += Application/inc/ Application/

# list all c and c++ source directories here
SRC_DIRS =  Drivers/CTS_NVM_Driver/ Drivers/CTS_NVM_Driver/Hardware/
SRC_DIRS += Drivers/DAC_Driver/ Drivers/DAC_Driver/Hardware/
SRC_DIRS += Drivers/GPIO_Touch_Driver/ Drivers/GPIO_Touch_Driver/Hardware/
SRC_DIRS += Drivers/RS232_Driver/ Drivers/RS232_Driver/Hardware/
SRC_DIRS += Drivers/SYSTIME_Driver/ Drivers/SYSTIME_Driver/Hardware/
SRC_DIRS += Drivers/STM32F0_Discovery/ Drivers/STM32F0_Discovery/Hardware/ 
SRC_DIRS += Drivers/STM32F0_Discovery/std_per_drv/src/ 
SRC_DIRS += Libs/TERM_Lib/ ./ Drivers/ Libs/
SRC_DIRS += Application/inc/ Application/

# list all assembly source directories here
ASM_DIRS = Drivers/STM32F0_Discovery/std_per_drv/src/ 

#generate lists of source files based on directory listings
C_SRC = $(wildcard $(addsuffix *.c,$(SRC_DIRS))) 
CPP_SRC = $(wildcard $(addsuffix *.cpp,$(SRC_DIRS)))
ASM_SRC = $(wildcard $(addsuffix *.s,$(ASM_DIRS)))

# define the output directory for the object files
OBJ_OUTPUT_DIR = Build/

# define the C object filenames to generate
OBJECT_NAMES = $(notdir $(C_SRC:.c=.o))
OBJECT_NAMES += $(notdir $(CPP_SRC:.cpp=.o))
OBJECT_NAMES += $(notdir $(ASM_SRC:.s=.o))
# add the output path to all the object names
OBJECTS = $(patsubst %.o, $(OBJ_OUTPUT_DIR)%.o, $(OBJECT_NAMES))

# build a search list for make dependencies
vpath %.c $(SRC_DIRS)
vpath %.cpp $(SRC_DIRS)
vpath %.s $(ASM_DIRS)
vpath %.h $(INCLUDE_DIRS)
vpath %.o

#  Debug Flags
DEBUGFLAGS = -O0 -g -gstabs+

#  Compiler Options
GCFLAGS = -isystem $(ISYSTEM_INC_DIR) -mthumb -mcpu=cortex-m0 -fno-builtin -Wall -std=gnu99 -fdata-sections -ffunction-sections
GCFLAGS += -DSTM32F0xx -DUSE_STDPERIPH_DRIVER $(DEBUGFLAGS)

# define any directories containing header files other than /usr/include
INCLUDES = $(addprefix -I,$(INCLUDE_DIRS)) 

#  Linker Options
LDFLAGS = -isystem $(ISYSTEM_INC_DIR)
LDFLAGS += -mthumb -mcpu=cortex-m0 -nostartfiles -fno-builtin -Wall -std=gnu99 -fdata-sections -ffunction-sections 
LDFLAGS += -DSTM32F0xx $(DEBUGFLAGS)
LDFLAGS += -T$(LINKER_SCRIPT) -Wl,-static -Wl,--gc-sections 

LDINCLUDES = 
LDLIBS = 

ASFLAGS = -mthumb -mcpu=cortex-m0 -g -gstabs+

#  Compiler/Assembler/Linker Paths
GCC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
RANLIB = arm-none-eabi-ranlib
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
REMOVE = rm -f
SIZE = arm-none-eabi-size
READELF = arm-none-eabi-readelf -a

#########################################################################

all:: begin gcc_version printvar $(PROJECT).hex $(PROJECT).bin $(PROJECT).readelf end

$(PROJECT).elf: $(notdir $(OBJECTS))
	@echo "Linking: " $(PROJECT).elf
	$(GCC) $(LDFLAGS) $(OBJECTS) -o $(OBJ_OUTPUT_DIR)$(PROJECT).elf $(LDLIBS) -Wl,-Map,$(OBJ_OUTPUT_DIR)$(PROJECT).map
	@echo 'Finished building target: $@'
	@echo 'Listing size: '
	$(MAKE) size
	@echo 'Creating list file: '
	$(MAKE) list	

$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -R .stack -O ihex $(OBJ_OUTPUT_DIR)$(PROJECT).elf $(OBJ_OUTPUT_DIR)$(PROJECT).hex

$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary -j .text -j .data $(OBJ_OUTPUT_DIR)$(PROJECT).elf $(OBJ_OUTPUT_DIR)$(PROJECT).bin

$(PROJECT).lst: $(PROJECT).elf
	$(OBJDUMP) -h -S $(PROJECT).elf > $(PROJECT).lst

stats: $(PROJECT).elf
	$(SIZE) $(OBJ_OUTPUT_DIR)$(PROJECT).elf

.PHONY: clean
clean:
	$(REMOVE) $(OBJ_OUTPUT_DIR)*.o
	$(REMOVE) $(OBJ_OUTPUT_DIR)$(PROJECT).hex
	$(REMOVE) $(OBJ_OUTPUT_DIR)$(PROJECT).elf
	$(REMOVE) $(OBJ_OUTPUT_DIR)$(PROJECT).map
	$(REMOVE) $(OBJ_OUTPUT_DIR)$(PROJECT).bin
	$(REMOVE) $(OBJ_OUTPUT_DIR)$(PROJECT).readelf
	$(REMOVE) $(OBJ_OUTPUT_DIR)*.lst
	
# Eye Candy
.PHONY begin: 
begin: 
	@echo
	@echo "---------- begin ----------"

.PHONY end:
end:
	@echo
	@echo "----------- end -----------"

.PHONY size:	
size:
	@echo
	$(SIZE) $(OBJ_OUTPUT_DIR)$(PROJECT).elf
	@echo	

.PHONY list:
list:
	@echo
	$(OBJDUMP) -h -S -D $(OBJ_OUTPUT_DIR)$(PROJECT).elf > "$(OBJ_OUTPUT_DIR)$(PROJECT).lst" 
	@echo
	
$(PROJECT).readelf: $(PROJECT).elf
	$(READELF) $(OBJ_OUTPUT_DIR)$(PROJECT).elf > $(OBJ_OUTPUT_DIR)$(PROJECT).readelf

.PHONY gcc_version: 
gcc_version: 
	@echo
	@$(GCC) --version
	
.PHONY:printvar
printvar: 
	@echo $(OBJECTS)
	
#########################################################################
#  Default rules to compile .c and .cpp file to .o
#  and assemble .s files to .o

# General Rule for compiling C source files
%.o : %.c
	@echo
	@echo "Compiling: " $(notdir $<)
	$(GCC) $(GCFLAGS) $(INCLUDES) -c $< -o $(OBJ_OUTPUT_DIR)$(notdir $@)
	@echo "Compiling Done: " $(notdir $<)
  
%.o : %.s
	@echo "Assemble: " $(notdir $<)
	$(AS) $(ASFLAGS) -o $(OBJ_OUTPUT_DIR)$(notdir $@) -c $<
	@echo "Assemble Done: " $(notdir $<)

#########################################################################