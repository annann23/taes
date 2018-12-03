##############################################################################
## compiler-opts.mak - compiler definitions and options for building the cFE 
##
## Target: ARM Buildroot
##
## Modifications:
##
###############################################################################
## 
## Warning Level Configuration
##
# WARNINGS=-Wall -ansi -pedantic -Wstrict-prototypes
WARNINGS=-Wall -Wstrict-prototypes

## 
## Host OS Include Paths ( be sure to put the -I switch in front of each directory )
##
SYSINCS=

##
## Target Defines for the OS, Hardware Arch, etc..
##
#TARGET_DEFS+=-D__ix86__ -D_ix86_ -D_LINUX_OS_  -D$(OS) -DX86PC -DBUILD=$(BUILD) -D_REENTRANT -D _EMBED_ 
#TARGET_DEFS+=-D_LINUX_OS_ -D$(OS) -DBUILD=$(BUILD) -D_REENTRANT -D _EMBED_ 
TARGET_DEFS+=-D_LINUX_OS_ -D$(OS) -D_REENTRANT -D _EMBED_ 

## 
## Endian Defines
##
ENDIAN_DEFS=-D_EL -DENDIAN=_EL -DSOFTWARE_LITTLE_BIT_ORDER 

##
## Compiler Architecture Switches
## 
ARCH_OPTS = -march=armv8-a -mtune=cortex-a53 -mfpu=neon -mfloat-abi=hard -fPIC

##
## Application specific compiler switches 
##
ifeq ($(BUILD_TYPE),CFE_APP)
   APP_COPTS=
   APP_ASOPTS=
else
   APP_COPTS=
   APP_ASOPTS=
endif

##
## Extra Cflags for Assembly listings, etc.
##
LIST_OPTS = 

##
## gcc options for dependancy generation
## 
COPTS_D = $(APP_COPTS) $(ENDIAN_DEFS) $(TARGET_DEFS) $(ARCH_OPTS) $(SYSINCS) $(WARNINGS)

## 
## General gcc options that apply to compiling and dependency generation.
##
COPTS=$(LIST_OPTS) $(COPTS_D)

##
## Extra defines and switches for assembly code
##
ASOPTS = $(APP_ASOPTS) -P -xassembler-with-cpp 

##---------------------------------------------------------
## Application file extention type
## This is the defined application extention.
## Known extentions: Mac OS X: .bundle, Linux: .so, RTEMS:
##   .s3r, vxWorks: .o etc.. 
##---------------------------------------------------------
APP_EXT = so

#################################################################################
## Host Development System and Toolchain defintions
##

##
## Host OS utils
##
RM=rm -f
CP=cp

##
## Compiler tools
##
COMPILER=arm-linux-gnueabihf-gcc
COMPILER_CC=arm-linux-gnueabihf-g++
ASSEMBLER=/usr/arm-linux-gnueabihf/bin/as
LINKER=/usr/arm-linux-gnueabihf/bin/ld -marmelf_linux_eabi 
AR=/usr/arm-linux-gnueabihf/bin/ar
NM=/usr/arm-linux-gnueabihf/bin/nm
SIZE=size
OBJCOPY=objcopy
OBJDUMP=objdump
TABLE_BIN = elf2cfetbl
