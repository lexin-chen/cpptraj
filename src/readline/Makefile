# Makefile for readline bundled with cpptraj.
include ../../config.h
DEL_FILE      = /bin/rm -f
AR            = ar cqs
TARGET        = libreadline.a

SOURCES       = bind.c \
		callback.c \
		compat.c \
		complete.c \
		display.c \
		funmap.c \
		histexpand.c \
		histfile.c \
		history.c \
		histsearch.c \
		input.c \
		isearch.c \
		keymaps.c \
		kill.c \
		macro.c \
		mbutil.c \
		misc.c \
		nls.c \
		parens.c \
		readline.c \
		rltty.c \
		savestring.c \
		search.c \
		shell.c \
		signals.c \
		termcap.c \
		terminal.c \
		text.c \
		tilde.c \
		tparam.c \
		undo.c \
		util.c \
		version.c \
		vi_mode.c \
		xmalloc.c

OBJECTS=$(SOURCES:.c=.o)

.c.o:
	$(VB)echo [READLINE] CC $<
	$(VB)$(CC) $(LFS) -DHAVE_CONFIG_H -I. $(CFLAGS) -c -o $@ $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	-$(DEL_FILE) $(TARGET)
	$(AR) $(TARGET) $(OBJECTS)

uninstall: clean

clean:
	$(DEL_FILE) *.o $(TARGET)

# Dependencies
bind.o : bind.c ansi_stdlib.h config.h history.h posixjmp.h posixstat.h rlconf.h rldefs.h rlprivate.h rlshell.h rlstdc.h rltypedefs.h xmalloc.h
callback.o : callback.c ansi_stdlib.h config.h posixjmp.h rlconf.h rldefs.h rlprivate.h rlstdc.h xmalloc.h
compat.o : compat.c rlstdc.h rltypedefs.h
complete.o : complete.c ansi_stdlib.h config.h posixdir.h posixjmp.h posixstat.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlstdc.h xmalloc.h
display.o : display.c ansi_stdlib.h config.h history.h posixjmp.h posixstat.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlstdc.h rltty.h rltypedefs.h rlwinsize.h tcap.h xmalloc.h
emacs_keymap.o : emacs_keymap.c
funmap.o : funmap.c ansi_stdlib.h rlconf.h rlstdc.h xmalloc.h
histexpand.o : histexpand.c ansi_stdlib.h histlib.h history.h rlmbutil.h rlshell.h rlstdc.h rltypedefs.h xmalloc.h
histfile.o : histfile.c ansi_stdlib.h histlib.h history.h posixstat.h rlshell.h rlstdc.h rltypedefs.h xmalloc.h
history.o : history.c ansi_stdlib.h histlib.h history.h rlstdc.h rltypedefs.h xmalloc.h
histsearch.o : histsearch.c ansi_stdlib.h histlib.h history.h rlstdc.h rltypedefs.h
input.o : input.c ansi_stdlib.h config.h posixjmp.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlshell.h rlstdc.h xmalloc.h
isearch.o : isearch.c ansi_stdlib.h config.h history.h posixjmp.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlstdc.h rltypedefs.h xmalloc.h
keymaps.o : keymaps.c ansi_stdlib.h emacs_keymap.c rlconf.h rlstdc.h vi_keymap.c xmalloc.h
kill.o : kill.c ansi_stdlib.h config.h history.h posixjmp.h rlconf.h rldefs.h rlprivate.h rlstdc.h rltypedefs.h xmalloc.h
macro.o : macro.c ansi_stdlib.h config.h history.h posixjmp.h rlconf.h rldefs.h rlprivate.h rlstdc.h rltypedefs.h xmalloc.h
mbutil.o : mbutil.c ansi_stdlib.h config.h posixjmp.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlstdc.h xmalloc.h
misc.o : misc.c ansi_stdlib.h config.h history.h posixjmp.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlshell.h rlstdc.h rltypedefs.h xmalloc.h
nls.o : nls.c ansi_stdlib.h config.h posixjmp.h rlconf.h rldefs.h rlprivate.h rlshell.h rlstdc.h
parens.o : parens.c posixjmp.h rlconf.h rlprivate.h rlstdc.h
readline.o : readline.c ansi_stdlib.h config.h history.h posixjmp.h posixstat.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlshell.h rlstdc.h rltypedefs.h xmalloc.h
rltty.o : rltty.c config.h posixjmp.h rlconf.h rldefs.h rlprivate.h rlstdc.h rltty.h rlwinsize.h
savestring.o : savestring.c rlstdc.h xmalloc.h
search.o : search.c ansi_stdlib.h config.h history.h posixjmp.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlstdc.h rltypedefs.h xmalloc.h
shell.o : shell.c ansi_stdlib.h rlshell.h rlstdc.h xmalloc.h
signals.o : signals.c config.h history.h posixjmp.h rlconf.h rldefs.h rlprivate.h rlstdc.h rltypedefs.h
termcap.o : termcap.c
terminal.o : terminal.c ansi_stdlib.h config.h history.h posixjmp.h posixstat.h rlconf.h rldefs.h rlprivate.h rlshell.h rlstdc.h rltty.h rltypedefs.h rlwinsize.h tcap.h xmalloc.h
text.o : text.c ansi_stdlib.h config.h history.h posixjmp.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlshell.h rlstdc.h rltypedefs.h xmalloc.h
tilde.o : tilde.c ansi_stdlib.h rlstdc.h tilde.h xmalloc.h
tparam.o : tparam.c
undo.o : undo.c ansi_stdlib.h config.h history.h posixjmp.h rlconf.h rldefs.h rlprivate.h rlstdc.h rltypedefs.h xmalloc.h
util.o : util.c ansi_stdlib.h config.h posixjmp.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlstdc.h xmalloc.h
version.o : version.c
vi_keymap.o : vi_keymap.c
vi_mode.o : vi_mode.c ansi_stdlib.h config.h history.h posixjmp.h rlconf.h rldefs.h rlmbutil.h rlprivate.h rlstdc.h rltypedefs.h xmalloc.h
xmalloc.o : xmalloc.c ansi_stdlib.h rlstdc.h xmalloc.h
