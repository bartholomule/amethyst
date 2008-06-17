SYMLINKS = graphics general math

BUILT_FILES = $(symlinks)

$(SYMLINKS):
	rm -f $@ && ln -s $(top_srcdir)/src/reference_material/$@ $@

all-local: $(SYMLINKS)

