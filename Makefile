ifeq ($(shell uname),Linux)
OS                     = linux
else
OS                     = win64
endif

Packages 		       = \
	                      Lib \
	                      Calc \

Empty                  =
Space                  = $(Empty) $(Empty)
temproot               = $(subst /Source,$(Space),$(CURDIR))
DEV_ROOT               = $(word 1,$(temproot))

ObjectFiles            = $(wildcard Object/$(OBJMID)/*/*.o) 
MainProg               = main.c
Main                   = capture

PackageListLoop        = $(patsubst %,Source/%/.loop,$(Packages))

ifeq ($(OS),win32)
OBJMID                 = win32
LinkerOption           = $(CLIBS)
makelinux:
	@echo "Start Make Window32"
endif

ifeq ($(OS),win64)
OBJMID                 = win64
LinkerOption           = $(CLIBS)
makelinux:
	@echo "Start Make Window64"
endif
ifeq ($(OS),linux)
OBJMID                 = linux
LinkerOption           = `pkg-config --cflags --libs gtk+-3.0 gmodule-2.0` -lm -lmysqlclient
COPTION                = `pkg-config --cflags --libs gtk+-3.0 gmodule-2.0`
makelinux:
	@echo "Start Make Linux"
endif


%.loop:
	@$(MAKE) $(MakeOptions) -C $(subst .loop,,$@) -f Make package_$(MAKECMDGOALS)

build: makelinux $(PackageListLoop)
	@echo "End Make"
	@gcc -c Source/main.c -o Object/$(OBJMID)/main.o $(COPTION)
	@gcc -g -o $(DEV_ROOT)/Binary/$(Main).exe $(ObjectFiles) Object/$(OBJMID)/main.o $(LinkerOption)
	$(DEV_ROOT)/Binary/$(Main).exe

clean: $(PackageListLoop)
