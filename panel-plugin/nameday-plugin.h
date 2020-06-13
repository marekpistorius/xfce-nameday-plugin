/*  $Id$
 *
 *  Copyright (c) mmaniu
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __NAMEDAY_H__
#define __NAMEDAY_H__

G_BEGIN_DECLS

enum {
	TYPE_STR = 0,
};

/* plugin structure */
typedef struct
{
    XfcePanelPlugin *plugin;

    /* panel widgets */
    GtkWidget       *ebox;
    GtkWidget       *hvbox;
    GtkWidget       *label;
	/* Dialogs Widgets */
	GtkWidget		*check1;
	GtkWidget		*check2;
	GtkWidget		*combo;
	GtkWidget       *spin;

	#if !GTK_CHECK_VERSION(2,12,0)
		GtkTooltips     *tooltips;
	#endif

	gint 		updatetimeout;
    /* settings */
	gchar        *setting1;
	gint		 setcount;

}
NamedaysPlugin;


void
nameday_save (XfcePanelPlugin *plugin,
             NamedaysPlugin *nmday);

gchar * load_nm(GDate *date,NamedaysPlugin *nmday);


G_END_DECLS

#endif /* !__NAMEDAY_H__ */
