/* 
* (C) 2024-2025 Marek Pistorius
* This file is part of xfce-nameday-plugin.
* xfce-nameday-plugin is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* xfce-nameday-plugin is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Xfce-nameday-plugin. If not, see <https://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <string.h>
#include <gtk/gtk.h>

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/xfce-panel-plugin.h>

#include "nameday-plugin.h"
#include "nameday-plugin-dialogs.h"
#include "nameday-plugin-callendar.h"

static gchar* onDetailFuncs(GtkCalendar *calendar, guint year, guint month, guint day, gpointer data)
{
	g_autoptr(GDate) date = g_date_new_dmy(day, month, year);
	return g_strdup_printf(_("Nameday have %s "),load_nm(date,(NamedaysPlugin *)data));
}

static void onDaySelected(GtkCalendar *calendar, gpointer data)
{
	nameday_calendar *nmd_cal = (nameday_calendar*)data;
	guint year = 0 ,month = 0,day = 0;
	gtk_calendar_get_date(calendar,&year,&month,&day);
	g_autoptr(GDate) date = g_date_new_dmy(day,month, year);
	gtk_label_set_text(GTK_LABEL(nmd_cal->label), load_nm(date,nmd_cal->plugin));
}

void initDialogWithCalandar(XfcePanelPlugin *plugin, NamedaysPlugin *nm)
{
	GtkWidget *dlg,*box;
	nameday_calendar *nmd_cal;
	nmd_cal = g_slice_new0(nameday_calendar);
	nmd_cal->plugin = nm;
	nmd_cal->main_widget = gtk_vbox_new(FALSE,0);

	dlg = gtk_dialog_new_with_buttons(_("Nameday Plugin Calendar"),
                                                GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (plugin))),
                                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                                _("Close"), GTK_RESPONSE_OK,
									NULL);

	nmd_cal->calendar = gtk_calendar_new();
	nmd_cal->label = gtk_label_new("BirthDay =>");
	gtk_misc_set_padding (GTK_MISC(nmd_cal->label),0,0);
	gtk_misc_set_alignment (GTK_MISC(nmd_cal->label),0,0);
	gtk_box_pack_start(GTK_BOX(nmd_cal->main_widget), nmd_cal->calendar,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(nmd_cal->main_widget), nmd_cal->label,FALSE,FALSE,0);
	box = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
	gtk_container_add(GTK_CONTAINER(box),nmd_cal->main_widget);
	gtk_widget_show_all(nmd_cal->main_widget);
	gtk_calendar_set_detail_func(GTK_CALENDAR(nmd_cal->calendar),(GtkCalendarDetailFunc)onDetailFuncs,(gpointer)nm,NULL);

	g_autoptr(GDate) date = g_date_new();
	g_date_set_time_t(date,time(NULL));

	if(date) {
		gtk_calendar_mark_day(GTK_CALENDAR(nmd_cal->calendar),g_date_get_day(date));
	}

	g_signal_connect(nmd_cal->calendar,"day-selected",G_CALLBACK(onDaySelected),nmd_cal);

	int res = gtk_dialog_run(GTK_DIALOG(dlg));
	if(res == GTK_RESPONSE_OK);
	
	gtk_widget_destroy(dlg);

}
