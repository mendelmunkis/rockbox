#             __________               __   ___.
#   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
#   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
#   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
#   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
#                     \/            \/     \/    \/            \/
# $Id$
#

FREETYPE_DIR := $(ROOTDIR)/lib/freetype
FREETYPE_SRC := $(call preprocess, $(FREETYPE_DIR)/SOURCES)
FREETYPE_OBJ := $(call c2obj, $(FREETYPE_SRC))
LIBFREETYPE := $(BUILDDIR)/lib/freetype.a

OTHER_SRC += $(FREETYPE_SRC)
INCLUDES += -I$(FREETYPE_DIR)/include/

FTINCLUDES += -I$(FREETYPE_DIR)/include/ -I$(FREETYPE_DIR)/include/freetype/  -I$(FREETYPE_DIR)/include/freetype/internal/ -I$(FREETYPE_DIR)/include/freetype/internal/services/ -I$(FREETYPE_DIR)/include/freetype/config/

FREETYPEFLAGS = $(CFLAGS) -fstrict-aliasing -ffunction-sections $(SHARED_CFLAGS) -DFT2_BUILD_LIBRARY


# special rules for freetype
$(BUILDDIR)/lib/freetype/src/%.o: $(FREETYPE_DIR)/src/%.c
	$(SILENT)mkdir -p $(dir $@)
	$(call PRINTS,CC $(subst $(ROOTDIR)/,,$<))$(CC) -c $< -o $@ $(FTINCLUDES) $(FREETYPEFLAGS)

$(FREETYPE): $(FREETYPE_OBJ)
	$(SILENT)$(shell rm -f $@)
	$(call PRINTS,AR $(@F))$(AR) rcs $@ $^ >/dev/null
