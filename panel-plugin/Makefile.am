
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-DG_LOG_DOMAIN=\"libnameday\" \
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\" \
	$(PLATFORM_CPPFLAGS) 

plugindir = $(libdir)/xfce4/panel/plugins

plugin_LTLIBRARIES = \
	libnameday.la

libnameday_la_SOURCES = \
	nameday-definitions.h \
	nameday-utils.c \
	nameday-utils.h \
	nameday-plugin.c							\
	nameday-plugin.h							\
	nameday-plugin-data.c						\
	nameday-plugin-data.h						\
	nameday-plugin-dialogs.c					\
	nameday-plugin-dialogs.h					\
	nameday-plugin-callendar.c					\
	nameday-plugin-callendar.h					
	nameday-utils.h	\
	nameday-utils.c
	
libnameday_la_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(CAIRO_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4PANEL_CFLAGS)	\
	$(EXO_CFLAGS) \
	$(XFCONF_CFLAGS) \
	$(PLATFORM_CFLAGS)

libnameday_la_LDFLAGS =	 \
	-avoid-version \
	-module \
	-no-undefined \
	-export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
	$(PLATFORM_LDFLAGS)

libnameday_la_LIBADD = \
	$(GTK_LIBS) \
	$(CAIRO_LIBS) \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(EXO_LIBS) \
	$(XFCONF_LIBS) \
	$(LIBXFCE4PANEL_LIBS) \
	-lm

#
# .desktop file
#
desktopdir = $(datadir)/xfce4/panel/plugins
desktop_in_files =	nameday-plugin.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
	$(desktop_in_files)

DISTCLEANFILES = \
	$(desktop_DATA)

if MAINTAINER_MODE
BUILT_SOURCES = \
	$(libnameday_built_sources)

DISTCLEANFILES += \
	$(libnameday_built_sources)
endif

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
