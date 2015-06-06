###################################################################
#                                                                 #
# MAKEFILE for the 8051 micro-controller                          #
# Generates a hex file which can be downloaded using Flash Magic  #
# to the 8051 micro-controller via the serial port.               #
#                                                                 #
# Memory Model: SMALL                                             #
#                                                                 #
# Author: Dr. Leo Stocco, PhD, PEng                               #
#         The Department of Electrical & Computer Engineering     #
#         The University of British Columbia                      #
#                                                                 #
#         (C) COPYRIGHT Leo J. Stocco 2006                        #
###################################################################


###############
# Definitions #
###############

# PATHS
BIN    = ..\lib\bin

# FILES
SRC    = bootcode
TARGET = bootcode
REL    = $(TARGET).rel

# COMMANDS
CC     = sdcc
PACK   = packihx

# OPTIONS
COPT   = -c
MEM    = --model-small


##############
# Link Rules #
##############

# Convert to a hex file
$(TARGET).hex: $(TARGET).ihx
	$(PACK) $(TARGET).ihx > $(TARGET).hex

# Link rule
$(TARGET).ihx: $(REL)
	$(CC) -L$(BIN) $(MEM) $(REL)


#################
# Compile Rules #
#################

$(REL): $(SRC).c
	$(CC) $(COPT) $(MEM) -I$(BIN) -o$(REL) $(SRC).c


#################
# Generic Rules #
#################

all: clean $(TARGET).hex

clean:
	@if exist *.rel del *.rel
	@if exist *.ihx del *.ihx
	@if exist *.hex del *.hex
	@if exist *.sym del *.sym
	@if exist *.asm del *.asm
	@if exist *.rst del *.rst
	@if exist *.lst del *.lst
	@if exist *.adb del *.adb
	@if exist *.cdb del *.cdb
	@if exist *.mem del *.mem
	@if exist *.map del *.map
	@if exist *.lnk del *.lnk
