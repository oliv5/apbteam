LOGOS = logo logo_bw logo_hi logo_bw_hi logo_center logo_center_lowcolor \
	logo_center_bw avatar logo_icon_32

JPEGS = avatar
QUALITY = 85

all: $(LOGOS:%=apbteam_%.png) $(JPEGS:%=apbteam_%.jpeg)

logo_ID = g15885
logo_RES = -w 291 # -h 73
logo_bw_ID = g15872
logo_bw_RES = -w 291 # -h 73
logo_hi_ID = $(logo_ID)
logo_hi_RES = -d 180
logo_bw_hi_ID = $(logo_bw_ID)
logo_bw_hi_RES = -d 180
logo_center_ID = g16294
logo_center_RES = -w 320 # -h 185
logo_center_lowcolor_ID = g16091
logo_center_lowcolor_RES = -w 320 # -h 185
logo_center_bw_ID = g16084
logo_center_bw_RES = -w 320 # -h 185
avatar_ID = g13014
avatar_RES = -w 128 # -h 108
# Warning, icons are not build automaticaly.
logo_icon_32_ID = $(avatar_ID)
logo_icon_32_RES = -w 32

apbteam_%.png: apbteam_logo.svg
	inkscape -z -e $@ $($(*)_RES) -i $($(*)_ID) $<

%.jpeg: %.png
	convert -quality $(QUALITY) $< $@
