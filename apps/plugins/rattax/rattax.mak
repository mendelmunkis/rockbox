#             __________               __   ___.
#   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
#   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
#   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
#   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
#                     \/            \/     \/    \/            \/
# $Id$
#

RATTAXSRCDIR := $(APPSDIR)/plugins/rattax
RATTAXBUILDDIR := $(BUILDDIR)/apps/plugins/rattax

ROCKS += $(RATTAXBUILDDIR)/rattax.rock

RATTAX_SRC := $(call preprocess, $(RATTAXSRCDIR)/SOURCES)
RATTAX_OBJ := $(call c2obj, $(RATTAX_SRC))

# add source files to OTHER_SRC to get automatic dependencies
OTHER_SRC += $(RATTAX_SRC)

$(RATTAXBUILDDIR)/rattax.rock: $(RATTAX_OBJ)
