ifndef $(FOOTER_MK)
FOOTER_MK = 1

SRCS = $(wildcard *.c)
OBJS = $(subst .c,.o,$(SRCS))
OBJDIR=$(BASEDIR)/obj/$(MODULE)

LIB = lib$(MODULE).a
EXE = $(EXEDIR)/$(MODULE).out

# Rules
%.o: %.c
	@echo CC $(notdir $@)
	@$(CC) $(CCOPTS) $(DEFINES) $(INCLUDE) -c -o $(OBJDIR)/$@ $<

lib: $(LIBDIR)/$(LIB)

$(LIBDIR)/$(LIB): $(OBJS)
	@echo AR $(notdir $@)
	@$(AR) $(AROPTS) $@ $(OBJDIR)/*.o

install:

clean:
	@echo RM $(LIB)
	@rm -f $(LIBDIR)/$(LIB)
	@rm -f $(OBJDIR)/*.*

depend:
	$(shell [ -d ${OBJDIR} ] || mkdir -p ${OBJDIR})
	
exe:
	@echo LD $(notdir $(EXE))
	$(LD) $(LDOPTS) -o$(EXE) $(LIBS)
	
.PHONY : lib install clean depend exe

endif

