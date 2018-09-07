libs:
	@$(MAKE) -s -C drv  MODULE=drv $(TARGET)
	@$(MAKE) -s -C net  MODULE=net $(TARGET)
	@$(MAKE) -s -C osa  MODULE=osa $(TARGET)
	@$(MAKE) -s -C util MODULE=util $(TARGET)

all:
	@$(MAKE) -s libs
	
clean:
	@$(MAKE) -s libs TARGET=clean

install:
	@$(MAKE) -s libs TARGET=install
	
depend:
	@$(MAKE) -s libs TARGET=depend 
	
.PHONY : libs all clean depend

# End of file
