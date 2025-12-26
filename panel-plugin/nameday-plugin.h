/* 
* (C) 2024-2026 Marek Pistorius
* This file is part of xfce-nameday-plugin.
* xfce-nameday-plugin is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* xfce-nameday-plugin is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Xfce-nameday-plugin. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef __NAMEDAY_H__
#define __NAMEDAY_H__

G_BEGIN_DECLS

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

	gint 		updatetimeout;
    /* settings */
	gchar        *setting1;
	gint		setcount;
	GList*		 namedays_list;

}
NamedaysPlugin;


void
nameday_save (XfcePanelPlugin *plugin,
             NamedaysPlugin *nmday);

gchar * load_nm(GDate *date,NamedaysPlugin *nmday);


G_END_DECLS

#endif /* !__NAMEDAY_H__ */
