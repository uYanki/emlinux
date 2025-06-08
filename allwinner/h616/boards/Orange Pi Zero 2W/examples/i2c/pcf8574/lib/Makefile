##########################################################################
# Makefile
#
# This software is a devLib extension to wiringPi <http://wiringpi.com/>
# and enables it to control an LCD1602 or LCD2004 via a pcf8574 module
# http://ww1.microchip.com/downloads/en/devicedoc/20001952c.pdf
# 
#
# Copyright (c) 2019 Wade Ryan
#
# If you have questions or improvements email me at
# wryan67@gmail.com
#
# This software is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The given code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You can view the contents of the licence at <http://www.gnu.org/licenses/>.
##########################################################################


DYN_VERS_MAJ=1
DYN_VERS_MIN=0

VERSION=$(DYN_VERS_MAJ).$(DYN_VERS_MIN)
DESTDIR=/usr
PREFIX=/local

STATIC=libwiringPiLCD1602pi.a
DYNAMIC=libwiringPiLCD1602pi.so.$(VERSION)

# Use this line for debugging:
# DEBUG	= -g -O0 -rdynamic
# Production
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I.
CFLAGS	= $(DEBUG) -Wformat=2 -Wall $(INCLUDE) -Winline -pipe -fPIC

LIBS    =

###############################################################################

SRC	= lcd1602pi.c 

OBJ	=	$(SRC:.c=.o)

all:		$(DYNAMIC)

static:		$(STATIC)

$(STATIC):	$(OBJ)
	@echo "[Link (Static)]"
	@ar rcs $(STATIC) $(OBJ)
	@ranlib $(STATIC)
#	@size   $(STATIC)

$(DYNAMIC):	$(OBJ)
	@echo "[Link (Dynamic)]"
	@$(CC) -shared -Wl,-soname,libwiringPiLCD1602pi.so -o libwiringPiLCD1602pi.so.$(VERSION) -lpthread $(OBJ)

.c.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo "[Clean]"
	@rm -f $(OBJ) $(OBJ_I2C) *~ core tags Makefile.bak libwiringPiLCD1602pi.*

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)


install-headers:
	@echo "[Install Headers]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/include
	@install -m 0644 lcd1602pi.h		$(DESTDIR)$(PREFIX)/include

install:	$(DYNAMIC) install-headers
	@echo "[Install Dynamic Lib]"
	@install -m 0755 -d						$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libwiringPiLCD1602pi.so.$(VERSION)			$(DESTDIR)$(PREFIX)/lib/libwiringPiLCD1602pi.so.$(VERSION)
	@ln -sf $(DESTDIR)$(PREFIX)/lib/libwiringPiLCD1602pi.so.$(VERSION)	$(DESTDIR)/lib/libwiringPiLCD1602pi.so
	@ldconfig

install-static:	$(STATIC) install-headers
	@echo "[Install Static Lib]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libwiringPiLCD1602pi.a	$(DESTDIR)$(PREFIX)/lib

uninstall:
	@echo "[UnInstall]"
	@rm -f $(DESTDIR)$(PREFIX)/include/lcd1602pi.h
	@rm -f $(DESTDIR)$(PREFIX)/lib/libwiringPiLCD1602pi.*
	@ldconfig


depend:
	makedepend -Y $(SRC)

libwiringPiLCD1602pi.o: lcd1602pi.h
