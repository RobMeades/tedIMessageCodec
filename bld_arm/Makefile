C027N_SUPPORT_PRE = ../../u-blox/c027n_support
MBED_PRE = ../../third_party/mbed/mbed-src
GCC_BIN = c:/u-blox/gallery/oss/gcc4arm/win/4.9.3/bin
SRC_DIR = src
API_DIR = api
OBJ_DIR = obj
PROJECT = teddy_msg_codec
INCLUDE_PATHS = -I$(API_DIR) -I$(SRC_DIR)
INCLUDE_PATHS += -I$(C027N_SUPPORT_PRE) -I$(MBED_PRE) -I$(MBED_PRE)/common -I$(MBED_PRE)/hal -I$(MBED_PRE)/api -I$(MBED_PRE)/targets -I$(MBED_PRE)/targets/cmsis -I$(MBED_PRE)/targets/cmsis/TARGET_NXP -I$(MBED_PRE)/targets/cmsis/TARGET_NXP/TARGET_LPC176X -I$(NMBED_PRE)/targets/cmsis/TARGET_NXP/TARGET_LPC176X/TOOLCHAIN_GCC_ARM -I$(MBED_PRE)/targets/hal -I$(MBED_PRE)/targets/hal/TARGET_NXP -I$(MBED_PRE)/targets/hal/TARGET_NXP/TARGET_LPC176X -I$(MBED_PRE)/targets/hal/TARGET_NXP/TARGET_LPC176X/TARGET_UBLOX_C027
CPP_FILES := $(SRC_DIR)/teddy_msg_codec.cpp
O_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP_FILES))

############################################################################### 
AS      = $(GCC_BIN)/arm-none-eabi-as
AR      = $(GCC_BIN)/arm-none-eabi-ar
CC      = $(GCC_BIN)/arm-none-eabi-gcc
CPP     = $(GCC_BIN)/arm-none-eabi-g++

CPU = -mcpu=cortex-m3 -mthumb
CC_FLAGS = $(CPU) -c -g -fno-common -fmessage-length=0 -Wall -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer
CC_FLAGS += -MMD -MP
CC_SYMBOLS = -DTARGET_UBLOX_C027 -DTARGET_M3 -DTARGET_CORTEX_M -DTARGET_NXP -DTARGET_LPC176X -DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC -D__CORTEX_M3 -DARM_MATH_CM3 -DMBED_BUILD_TIMESTAMP=1418808562.31 -D__MBED__=1 -DTARGET_LPC1768 -DTARGET_FF_ARDUINO

all: $(PROJECT).lib 

# These are the pattern matching rules.
$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) $< -o $@

%:$(SRC_DIR)/%.cpp
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -o $@ $<
	
clean:
	rm -f $(OBJ_DIR)\*.d $(OBJ_DIR)\*.o $(PROJECT).lib $(DEPS)

.PHONY: clean

$(PROJECT).lib: $(O_FILES)
	$(AR) -r $@ $(O_FILES)

DEPS = $(.o=.d)
-include $(DEPS)