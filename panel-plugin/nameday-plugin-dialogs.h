/* 
* (C) 2024-2025 Marek Pistorius
* This file is part of xfce-nameday-plugin.
* xfce-nameday-plugin is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* xfce-nameday-plugin is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Xfce-nameday-plugin. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef __NAMEDAY_DIALOGS_H__
#define __NAMEDAY_DIALOGS_H__

G_BEGIN_DECLS

typedef struct {
	GtkWidget *dialog;
	NamedaysPlugin *nmdplg;
} 
xfcenameday_dialog;

void
nameday_configure    (XfcePanelPlugin *plugin,
                     NamedaysPlugin  *svt);

void
nameday_about        (XfcePanelPlugin *plugin);

xfcenameday_dialog *create_config_dialog(NamedaysPlugin *data, GtkWidget *vbox);

void
set_callback_config_dialog (xfcenameday_dialog * dialog,
                            void (cb) (NamedaysPlugin *));
                            
void apply_options (xfcenameday_dialog *dialog);                            

G_END_DECLS

#endif
