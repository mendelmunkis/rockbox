#             __________               __   ___.
#   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
#   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
#   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
#   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
#                     \/            \/     \/    \/            \/
# $Id$
#

PAINTRUNSRCDIR := $(APPSDIR)/plugins/paintrun
PAINTRUNBUILDDIR := $(BUILDDIR)/apps/plugins/paintrun

INCLUDES += -I$(PAINTRUNSRCDIR)/src \
            -I$(PAINTRUNSRCDIR)/platforms

ROCKS += $(PAINTRUNBUILDDIR)/paintrun.rock

PAINTRUN_SRC := $(call preprocess, $(PAINTRUNSRCDIR)/SOURCES)
PAINTRUN_OBJ := $(call c2obj, $(PAINTRUN_SRC))

# add source files to OTHER_SRC to get automatic dependencies
OTHER_SRC += $(PAINTRUN_SRC)

PAINTRUNCFLAGS = $(PLUGINFLAGS) -std=gnu99 -O2

$(PAINTRUNBUILDDIR)/paintrun.rock: $(PAINTRUN_OBJ)

# new rule needed to use extra compile flags
$(PAINTRUNBUILDDIR)/%.o: $(PAINTRUNSRCDIR)/%.c
	$(SILENT)mkdir -p $(dir $@)
	$(call PRINTS,CC $(subst $(ROOTDIR)/,,$<))$(CC) $(PAINTRUNCFLAGS) -c $< -o $@
