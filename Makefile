#FORWINDOW              = TRUE

Packages 		       = \
	                      Lib \

Empty                  =
Space                  = $(Empty) $(Empty)
temproot               = $(subst /Source,$(Space),$(CURDIR))
DEV_ROOT               = $(word 1,$(temproot))

ObjectFiles            = $(wildcard Object/*/*.o) 
MainProg               = main.c
Main                   = capture


PackageListLoop        = $(patsubst %,Source/%/.loop,$(Packages))

ifdef FORWINDOW
LinkerOption           = $(CLIBS) -lgsl -lgslcblas
makelinux:
	@echo "Start Make Linux"
else
LinkerOption           = `pkg-config --cflags --libs gtk+-3.0 gmodule-2.0` -lm -lmysqlclient
COPTION                = `pkg-config --cflags --libs gtk+-3.0 gmodule-2.0`
makelinux:
	@echo "Start Make Linux"
	@cd Binary; glib-compile-resources resource.xml
endif


%.loop:
	@$(MAKE) $(MakeOptions) -C $(subst .loop,,$@) -f Make package_$(MAKECMDGOALS)

build: makelinux $(PackageListLoop)
	@echo "End Make Linux"
	@gcc -c Source/main.c -o Object/main.o $(COPTION)
	@gcc -g -o $(DEV_ROOT)/Binary/$(Main).exe $(ObjectFiles) Object/main.o $(LinkerOption)
	$(DEV_ROOT)/Binary/$(Main).exe

clean: $(PackageListLoop)
