# Makefile for the web pages of the Puzzle compiler components

#POVRAY=x-povray
POVRAY=povray
PNG2JPG=convert -format jpeg
POVFLAGS=-P -D +FN
HTMLDIR=/home/falcon/reeuwijk/public_html

LOGOS = logo_spar.pov logo_rotan.pov logo_tm.pov logo_hazel.pov \
 logo_timber.pov logo_automap.pov logo_vnus.pov
ICONS = icon_spar.pov icon_rotan.pov icon_tm.pov icon_hazel.pov \
 icon_timber.pov icon_automap.pov icon_vnus.pov timber_bullet.pov
POVS = $(LOGOS) $(ICONS)
PNGS = ${POVS:.pov=.png}
JPGS = ${POVS:.pov=.jpg}

%.png: %.pov %.ini
	$(POVRAY) $*.ini $(POVFLAGS) +I$*.pov +O$*.png

%.jpg: %.png
	$(PNG2JPG) $*.png $*.jpg

all: $(JPGS)

clean:
	rm -f $(PNGS) $(JPGS)

empty:
	rm -f $(PNGS) $(JPGS)

.SECONDARY: $(PNGS)

$(PNGS): puzzle.inc pascal.inc
