/*  $Id$
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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <string.h>
#include <gtk/gtk.h>

#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/libxfce4panel.h>
#include "nameday-plugin.h"
#include "nameday-plugin-data.h"
#include "nameday-plugin-dialogs.h"
#include "nameday-utils.h"
#include "nameday-definitions.h"

/* Border? */
#define BORDER           8

typedef void (*cb_function) (NamedaysPlugin *);
static cb_function cb = NULL;

void apply_options (xfcenameday_dialog *dialog)
{
	g_autofree gchar * tmp;
	g_autoptr(GDate) date = g_date_new();
	g_date_set_time_t (date, time (NULL));
	
	gchar* combo_set = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(dialog->nmdplg->combo));
	dialog->nmdplg->setting1 = g_strdup(combo_set);
	
	tmp = load_nm(date,dialog->nmdplg);
	
	if(G_LIKELY(tmp!= NULL))
	{
		gtk_label_set_text(GTK_LABEL(dialog->nmdplg->label), tmp);
	}	
	else gtk_label_set_text(GTK_LABEL(dialog->nmdplg->label), "Bad");		
	
	if(cb)
		cb(dialog->nmdplg);
	
}

static void
nameday_configure_response (GtkWidget    *dialog,
                           gint          response,
                           xfcenameday_dialog *xfdialog)
{
  gboolean result;
  if (response == GTK_RESPONSE_HELP)
    {
      /* show help */
      //todo: change?
      result = g_spawn_command_line_async ("exo-open --launch WebBrowser " PLUGIN_WEBSITE, NULL);

      if (G_UNLIKELY (result == FALSE))
        g_warning (_("Unable to open the following url: %s"), PLUGIN_WEBSITE);
    }
  else
    {
      /*Apply setiings*/
      apply_options(xfdialog);
      
      /* remove the dialog data from the plugin */
      g_object_set_data (G_OBJECT (xfdialog->nmdplg->plugin), "dialog", NULL);
      
      /* unlock the panel menu */
      xfce_panel_plugin_unblock_menu (xfdialog->nmdplg->plugin);

      /* save the plugin */
      nameday_save (xfdialog->nmdplg->plugin, xfdialog->nmdplg);

      /* destroy the properties dialog */
      gtk_widget_destroy (dialog);
    }
}

xfcenameday_dialog * create_config_dialog(NamedaysPlugin *data, GtkWidget *vbox)
{
	xfcenameday_dialog *dialog;
	GtkWidget *label,*hbox,*label2;
	GList *list = NULL ,*stmp = NULL;
	gint i = 0;
	dialog = g_slice_new0(xfcenameday_dialog);
	
	dialog->nmdplg = (NamedaysPlugin *)data;
	dialog->dialog = gtk_widget_get_toplevel (vbox);
	dialog->nmdplg->spin =  gtk_spin_button_new_with_range(0,70,1);
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(dialog->nmdplg->spin),dialog->nmdplg->setcount);
	label = gtk_label_new(_("Namedays for :"));
	label2 = gtk_label_new (_("Set Count of Tooltips Namedays"));
	hbox = gtk_box_new (GTK_ORIENTATION_VERTICAL,BORDER);
	dialog->nmdplg->combo = gtk_combo_box_text_new();
    //TODO: not path directly?
	list = getdirlist(NAMEDAY_PATH_GLOBAL "/data/");

	for(stmp = list; stmp ;stmp = g_list_next(stmp),i++)
	{
		  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dialog->nmdplg->combo),(gchar *)stmp->data);
		  
		  if(strcmp(((gchar *)stmp->data),data->setting1) == 0)
				gtk_combo_box_set_active(GTK_COMBO_BOX(dialog->nmdplg->combo),i--);
				
		g_free(stmp->data);		
	}
	
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), dialog->nmdplg->combo, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), label2, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), dialog->nmdplg->spin, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
	
	gtk_widget_show_all(hbox);
	
	return dialog;	
}

static void update_config(NamedaysPlugin *data)
{
	gchar *text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(data->combo));
	if(G_UNLIKELY(text!= NULL))
		data->setting1 = g_strdup(text);
	gint count = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(data->spin));
	if(G_UNLIKELY(count < 0))
		data->setcount = count;
}

void nameday_configure(XfcePanelPlugin *plugin,
                  NamedaysPlugin *nmd)
{
  GtkWidget *dlg,*vbox;
  xfcenameday_dialog *dialog;

  /* block the plugin menu */
  xfce_panel_plugin_block_menu (plugin);

  /* create the dialog */
  dlg = gtk_dialog_new_with_buttons(_("Nameday Plugin"),
                                                GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (plugin))),
                                                GTK_DIALOG_DESTROY_WITH_PARENT ,         
                                                "gtk-help", GTK_RESPONSE_HELP,
                                                _("_Close"),_("_OK"),
                                                NULL);

  gtk_container_set_border_width (GTK_CONTAINER (dlg), 2);
  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL,BORDER);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), BORDER - 2);
  gtk_widget_show (vbox);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dlg))),vbox,TRUE,TRUE,0 );

  /* set dialog icon */
  //todo probaly?
  gtk_window_set_icon_name (GTK_WINDOW (dlg), "xfce4-settings");
  
  dialog = create_config_dialog (nmd, vbox);
   
  /* center dialog on the screen */
  gtk_window_set_position (GTK_WINDOW (dlg), GTK_WIN_POS_CENTER);

  /* link the dialog to the plugin, so we can destroy it when the plugin
   * is closed, but the dialog is still open */
  g_object_set_data (G_OBJECT (plugin), "dialog", dialog);

  /* connect the reponse signal to the dialog */
  g_signal_connect (G_OBJECT (dlg), "response",
                    G_CALLBACK(nameday_configure_response), dialog);

  set_callback_config_dialog (dialog, update_config);	
  /* show the entire dialog */
  gtk_widget_show (dlg);
}

void nameday_about (XfcePanelPlugin *plugin)
{
   	gtk_show_about_dialog(NULL, "copyright", _("Namedays applet for Xfce"),
								"license", "GPLv2, Me <maniu at pm dot me>,2020",
								"program-name", _("Namedays Plugin"),
								"version", VERSION,
								"website", PLUGIN_WEBSITE,
								NULL
								);
}

void set_callback_config_dialog (xfcenameday_dialog *dialog,
                           cb_function         cb_new)
{
  cb = cb_new;
}
