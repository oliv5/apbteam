EEPROM_DEFAULTS_new := $(EEPROM_DEFAULTS)
-include obj/eeprom_defaults
EXTRA_CLEAN_FILES += obj/eeprom_defaults
ifneq (was $(EEPROM_DEFAULTS_new),$(EEPROM_DEFAULTS_old))
obj/eeprom_defaults: | $(OBJDIR)
	echo "EEPROM_DEFAULTS_old = was $(EEPROM_DEFAULTS_new)" > $@
.PHONY: obj/eeprom_defaults
endif
obj/eeprom.avr.avr.o: obj/eeprom_defaults
