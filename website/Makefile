# Makefile for the Joses web site.
#

PERLFLAGS = -w

# Don't forget to update constants.pli!!!
SPARVERSION = 2.0.1
#SPARPREVERSION = 2.0
SPARLANGSPECVERSION = 2.0
#SPARLANGSPECPREVERSION = 2.0
KAFFEVERSION = 1.0.6

# .pl files for web pages.
PL_FILES = $(wildcard spar/*.pl rotan/*.pl vnus/*.pl)\
    automap/index.pl \
    documents.pl \
    people.pl \
    index.pl \
    news.pl \
    jacks.pl \
    related.pl \
    implementation.pl \
    downloading.pl \
    requirements.pl

OTHER_HTML_FILES = gpl.html lgpl.html

HTML_FILES = $(PL_FILES:.pl=.html) $(OTHER_HTML_FILES)

HELPER_FILES = structure.pli style.pli constants.pli

GRAPHICS_FILES = $(wildcard povray/*.jpg pictures/*.gif)

DOWNLOAD_FILES = $(wildcard publications/*.pdf publications/*.ps.gz presentations/*.ppt presentations/*.ppt.gz spar/spar-langspec-*.pdf spar/timber-*) \
    vnus/vnus-reference.ps.gz \
    vnus/vnus-reference.ps \
    spar/kaffe-$(KAFFEVERSION).tar.gz \
    timber.css

INSTALLFILES = $(HTML_FILES) $(GRAPHICS_FILES) $(DOWNLOAD_FILES)

JUNK = $(HTML_FILES)

all:
	cd povray; make all
	make local-all

local-all: $(HTML_FILES)


#tmversions.pli: $(MAKEFILE) Makefile
#	echo '$$tmkernelversion = "$(TMKERNEL_VERSION)";' > tmversions.pli
#	echo '$$tmpasversion = "$(TMPAS_VERSION)";' >> tmversions.pli
#	echo '$$tmmiraversion = "$(TMMIRA_VERSION)";' >> tmversions.pli
#	echo '$$tmdemoversion = "$(TMDEMO_VERSION)";' >> tmversions.pli

%.html: %.pl structure.pli
	perl $(PERLFLAGS) $< > $@

check: $(HTML_FILES)
	weblint -pedantic $(HTML_FILES)

checkbot: $(HTML_FILES) $(GRAPHICS_FILES) $(DOWNLOAD_FILES)
	checkbot -url http://www.pds.its.tudelft.nl --exclude ~mathijs/bookmarks

clean:
	rm -f $(JUNK)

install: $(INSTALLFILES) install.pl
	@echo "Installing..."
	@perl install.pl $(INSTALLDIR) $(INSTALLFILES)
	@echo "Done"

t:
	@echo $(GRAPHICS_FILES)

## Some dependencies
$(HTML_FILES): $(HELPER_FILES)
#software/Tm/downloading.html: tmversions.pli
#software/Tm/intro.html: software/Tm/tmflow.gif
