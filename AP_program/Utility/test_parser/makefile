MI_CROSS_COMPILER=/home/tomyhu1995/Documents/openwrt/OpenWrt-SDK-15.05.1-ramips-mt7620_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mipsel-openwrt-linux-
CC = ${MI_CROSS_COMPILER}gcc

STAGING_DIR=/home/tomyhu1995/Documents/openwrt/OpenWrt-SDK-15.05.1-ramips-mt7620_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2

EXE_NAME = TEST_UTILITY

INC = 
INC += -Ilib/ 
INC += -I.
INC += -I../
INC += -I../../FW_control/
INC += -I$(STAGING_DIR)/usr/include

SRC = 
SRC += ../funclog
SRC += ../bitmap_tool 
SRC += ../parser
SRC += ./test_parser
SRC += ../../FW_control/FW_control_mi

LIB =
LIB += -L$(STAGING_DIR)/usr/lib

LDFLAG =
LDFLAG += -liwinfo
LDFLAG += -luci

SRC_FILES = ${addsuffix .c, ${SRC}}
OBJ_FILES = ${addsuffix .o, ${SRC}}

all: ${EXE_NAME}

${EXE_NAME}: ${OBJ_FILES}
	${CC} ${CFLAGS} ${INC} ${LIB} ${OBJ_FILES} -o $@ ${LDFLAG} 
${OBJ_FILES} : %.o : %.c
	${CC} ${CFLAGS} -c ${INC} $< -o $@ ${LDFLAG}
clean:
	rm ${OBJ_FILES} ${EXE_NAME}
