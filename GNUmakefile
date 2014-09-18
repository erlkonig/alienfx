#	GNUmakefile : Compilation method configuration

#	@(#) GNUmakefile Copyright (c) 2010 C. Alex. North-Keys
#	$Group: Talisman $
#	$Incept: 2010-10-19 07:05:53 GMT (Oct Tue) 1287471953 $
#	$Source: /home/erlkonig/zoo/alienfx/GNUmakefile $
#	$State: $
#	$Revision: $
#	$Date: 2010-10-19 07:05:53 GMT (Oct Tue) 1287471953 $
#	$Author: erlkonig $

PROG = alienfx
SRCS = alienfx.c fgetstr.c stoss.c
HEADERS = fgetstr.h stoss.h
INSTALLS = \
	$(DESTBIN)/$(PROG) \
	$(DESTBIN)/alienfx-uptime \
	$(DESTMAN)/man1/$(PROG).1 \
	$(DESTMAN)/man1/alienfx-uptime.1

DISTVERS = 0.1
DISTADDS = README LICENSE alienfx-uptime man1/alienfx.1 man1/alienfx-uptime.1

include $(firstword $(GNUmakecore) GNUmakecore)

LDFLAGS  += -L$(DESTLIB)
CPPFLAGS += -I. -I$(DESTINC)
LDLIBS   += -lusb-1.0 $(LIBPTHREAD)

# -------------------------------------------------------------- eof #
