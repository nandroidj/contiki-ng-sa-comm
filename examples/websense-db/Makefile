CONTIKI_PROJECT = websense-db
all: $(CONTIKI_PROJECT)
CONTIKI = ../..

# The BR is either native or embedded, and in the latter case must support SLIP
PLATFORMS_EXCLUDE = nrf52dk z1

# Simplelink Platform
TARGET = simplelink
BOARD = launchpad/cc1352r1
CFLAGS += -g

# Include RPL BR module
#include $(CONTIKI)/Makefile.dir-variables
#MODULES += $(CONTIKI_NG_SERVICES_DIR)/rpl-border-router

# Include webserver module
MODULES_REL += webserver
# Include optional target-specific module
include $(CONTIKI)/Makefile.identify-target
MODULES_REL += $(TARGET)

include $(CONTIKI)/Makefile.include
