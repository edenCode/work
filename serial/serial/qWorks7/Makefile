# Application home directory
TOPDIR := $(CURDIR)

# Load global configuration
include Rules.mak

# Target to build
all:
	@$(MAKE) -s clean
	@$(MAKE) -s depend
	@$(MAKE) -s libs
	
libs:
	@$(MAKE) -s -C $(HOMEDIR)/src $(TARGET)

install:
	@$(MAKE) -s libs TARGET=install

depend:
	$(shell [ -d ${LIBDIR} ] || mkdir -p ${LIBDIR})
	@$(MAKE) -s libs TARGET=depend 

clean:
	@$(MAKE) -s libs TARGET=clean
	
backup:
	F=`basename $(TOPDIR)` ; cd .. ; \
	gtar --force-local -zcvf `date "+$$F-%Y-%m-%d-%T.tar.gz"` $$F

.PHONY: exe all libs depend clean distclean backup

export TOPDIR

# End of file

