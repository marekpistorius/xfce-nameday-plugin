/*  $Id$
 *
 *  Copyright (c) 2017-2022
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
#ifdef HAVE_STRING_H
	#include <string.h>
#endif

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/libxfce4panel.h>

#include "nameday-plugin.h"
#include "nameday-utils.h"
#include "nameday-plugin-data.h"
#include "nameday-plugin-dialogs.h"
#include "nameday-plugin-callendar.h"
#include "nameday-definitions.h"


/* default settings */
#define DEFAULT_LANG NAMEDAY_PATH_GLOBAL "data/czech.dat"

#define DEFAULT_COUNT 5
#define UPDATE_TIME      1800

/* prototypes */
static gchar* getEaster(int year);
static gchar* getNamedayDay(gchar *_name, NamedaysPlugin *nmday);
static void onEasters(GtkWidget *widget, gpointer data);
static void onSearchNmD(GtkWidget *widget, gpointer data);
static void onCopy(GtkWidget *widget, gpointer data);
static void showDialog(gchar *tmp);
static gboolean on_tooltip_cb (GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, NamedaysPlugin *data) ;
static void onCalendar(GtkWidget *widget, gpointer data);

static void nameday_construct (XfcePanelPlugin *plugin);

/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER(nameday_construct);

void
nameday_save (XfcePanelPlugin *plugin,
             NamedaysPlugin    *nmday)
{
  
  /* get the config file location */
  g_autofree gchar  *file = xfce_panel_plugin_save_location (plugin, TRUE);

  if (G_UNLIKELY (file == NULL))
    {
       DBG ("Failed to open config file");
       return;
    }
  /* open the config file, read/write */
  XfceRc *rc = xfce_rc_simple_open (file, FALSE);

  if (G_LIKELY (rc != NULL))
    {
      /* save the settings */
      DBG(".");
      if (nmday->setting1)
        xfce_rc_write_entry    (rc, "lang", nmday->setting1);

	  	xfce_rc_write_int_entry (rc, "count", nmday->setcount);

      /* close the rc file */
      xfce_rc_close (rc);
    }
}

static void nameday_read (NamedaysPlugin *nmday)
{
  /* get the plugin config file location */
  g_autofree gchar  *file = xfce_panel_plugin_save_location (nmday->plugin, TRUE);

  if (G_LIKELY (file != NULL))
    {
      /* open the config file, readonly */
      XfceRc  *rc = xfce_rc_simple_open (file, TRUE);

      if (G_LIKELY (rc != NULL))
        {
          /* read the settings */
          const gchar *value = xfce_rc_read_entry (rc, "lang", DEFAULT_LANG);
          nmday->setting1 = g_strdup (value);
		      nmday->setcount =	xfce_rc_read_int_entry(rc, "count", DEFAULT_COUNT);

          /* cleanup */
          xfce_rc_close (rc);

          /* leave the function, everything went well */
          return;
        }
    }

  /* something went wrong, apply default values */
  DBG ("Applying default settings");

  nmday->setting1 = g_strdup (DEFAULT_LANG);
  nmday->setcount = DEFAULT_COUNT;	
}

gchar *load_nm(GDate *date, NamedaysPlugin *nmday)
{
	guint index;
	
	if(g_date_valid(date))
	{
		index = g_date_get_day_of_year(date);
		if( (index > 0) && (index < 367) )
		{
			DBG("Load");
			return (gchar*)g_list_nth (nmday->namedays_list,index);
		}
	}

	return NULL;	
}

static gboolean update_nameday(NamedaysPlugin *data)
{
	g_autofree gchar *tmp;
	gint hour = 0;
	g_autoptr(GDate) date = g_date_new();

  time_t now = time (NULL);
  if (now == (time_t) -1)
  {
      DBG("No Valid Date");
      return TRUE;
  }  

	g_date_set_time_t (date,now );

	tmp = load_nm(date,data);

	if(G_LIKELY(tmp != NULL))
	{
		gtk_label_set_text(GTK_LABEL(data->label),tmp);
	}	
	else 
	{
		gtk_label_set_text(GTK_LABEL(data->label),_("Don't go here"));	
	}	

	g_date_clear(date, 1);

	GDateTime* gtime = g_date_time_new_now_local(); 
	hour = g_date_time_get_hour(gtime);
	
  if(hour < 12)
	{
		data->updatetimeout = g_timeout_add_seconds((int)43200, (GSourceFunc) update_nameday, data);
		return TRUE;
	}
	if(hour > 12 && hour < 23)
	{
		data->updatetimeout = g_timeout_add_seconds((int)3600, (GSourceFunc) update_nameday, data);
		return TRUE;	
	}
	
  data->updatetimeout = g_timeout_add_seconds (UPDATE_TIME, (GSourceFunc) update_nameday, data);
	return TRUE;
}


static NamedaysPlugin *
nameday_new (XfcePanelPlugin *plugin)
{
  NamedaysPlugin   *svt;
  GtkOrientation  orientation;

  /* allocate memory for the plugin structure */
  svt = g_slice_new0 (NamedaysPlugin);

  /* pointer to plugin */
  svt->plugin = plugin;

  /* read the user settings */
  nameday_read (svt);

  /* get the current orientation */
  orientation = xfce_panel_plugin_get_orientation (plugin);

  /* create some panel widgets */
  svt->ebox = gtk_event_box_new ();
  gtk_widget_show (svt->ebox);

  svt->hvbox = gtk_box_new (orientation,  2);
  gtk_widget_show (svt->hvbox);
  gtk_container_add (GTK_CONTAINER (svt->ebox), svt->hvbox);

  /* widgets */
  svt->label = gtk_label_new (_("Namedays have "));
  gtk_widget_show (svt->label);
  gtk_box_pack_start (GTK_BOX (svt->hvbox), svt->label, FALSE, FALSE, 0);

  update_nameday(svt);

  return svt;
}

static void
nameday_free (XfcePanelPlugin *plugin,
             NamedaysPlugin *svt)
{
  GtkWidget *dialog;

  /* check if the dialog is still open. if so, destroy it */
  dialog = g_object_get_data (G_OBJECT (plugin), "dialog");
  if (G_UNLIKELY (dialog != NULL))
    gtk_widget_destroy (dialog);

  /* destroy the panel widgets */
  gtk_widget_destroy (svt->hvbox);

  /* cleanup the settings */
  if (G_LIKELY (svt->setting1 != NULL))
    g_free (svt->setting1);

  /* free the plugin structure */
  g_slice_free (NamedaysPlugin, svt);
}

static void
nameday_orientation_changed (XfcePanelPlugin *plugin,
                            GtkOrientation   orientation,
                            NamedaysPlugin    *svt)
{
  /* change the orienation of the box */
 gtk_orientable_set_orientation(GTK_ORIENTABLE(svt->hvbox), orientation);

}

static gboolean
nameday_size_changed (XfcePanelPlugin *plugin,
                     gint             size,
                     NamedaysPlugin   *svt)
{
  GtkOrientation orientation;

  /* get the orientation of the plugin */
  orientation = xfce_panel_plugin_get_orientation (plugin);

  /* set the widget size */
  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    gtk_widget_set_size_request (GTK_WIDGET (plugin), -1, size);
  else
    gtk_widget_set_size_request (GTK_WIDGET (plugin), size, -1);

  /* we handled the orientation */
  return TRUE;
}

void
nameday_construct (XfcePanelPlugin *plugin)
{
  NamedaysPlugin *nmday;
  GtkWidget *itemvel,*searchsvt,*copyItem,*calnItem;

  /* create the plugin */
  nmday = nameday_new (plugin);
  g_autoptr(GDate) date = g_date_new();
  g_date_set_time_t(date,time(NULL));
  gboolean bISLeap = g_date_is_leap_year(date->year);
  
	nmday->namedays_list = create_list_namedays(nmday->setting1);
  /* add the ebox to the panel */
  gtk_container_add (GTK_CONTAINER (plugin), nmday->ebox);

  /* show the panel's right-click menu on this ebox */
  xfce_panel_plugin_add_action_widget (plugin, nmday->ebox);

  itemvel = gtk_menu_item_new_with_label(_("Easter date"));
  xfce_panel_plugin_menu_insert_item (plugin,GTK_MENU_ITEM(itemvel));
  g_signal_connect(itemvel, "activate", G_CALLBACK(onEasters), nmday);
  gtk_widget_show(itemvel);

  searchsvt = gtk_menu_item_new_with_label(_("Search nameday"));
  xfce_panel_plugin_menu_insert_item(plugin, GTK_MENU_ITEM(searchsvt));
  g_signal_connect(searchsvt, "activate", G_CALLBACK(onSearchNmD), nmday);
  gtk_widget_show(searchsvt);
  /**/
  copyItem = gtk_menu_item_new_with_label(_("Copy Nameday"));
  xfce_panel_plugin_menu_insert_item(plugin, GTK_MENU_ITEM(copyItem));
  g_signal_connect(copyItem, "activate", G_CALLBACK(onCopy), nmday);
  gtk_widget_show(copyItem);
  
  calnItem	= gtk_menu_item_new_with_label(_("Calendar"));
  xfce_panel_plugin_menu_insert_item(plugin, GTK_MENU_ITEM(calnItem));
  g_signal_connect(calnItem, "activate", G_CALLBACK(onCalendar), nmday);
  gtk_widget_show(calnItem);

  g_object_set (G_OBJECT(nmday->ebox), "has-tooltip", TRUE, NULL);
  g_signal_connect(G_OBJECT(nmday->ebox), "query-tooltip",
		   G_CALLBACK(on_tooltip_cb),
		  nmday);

  /* connect plugin signals */
  g_signal_connect (G_OBJECT (plugin), "free-data",
                    G_CALLBACK (nameday_free), nmday);

  g_signal_connect (G_OBJECT (plugin), "save",
                    G_CALLBACK (nameday_save), nmday);

  g_signal_connect (G_OBJECT (plugin), "size-changed",
                    G_CALLBACK (nameday_size_changed), nmday);

  g_signal_connect (G_OBJECT (plugin), "orientation-changed",
                    G_CALLBACK (nameday_orientation_changed), nmday);

  /* show the configure menu item and connect signal */
  xfce_panel_plugin_menu_show_configure (plugin);
  g_signal_connect (G_OBJECT (plugin), "configure-plugin",
                    G_CALLBACK (nameday_configure), nmday);

  /* show the about menu item and connect signal */
  xfce_panel_plugin_menu_show_about (plugin);
  g_signal_connect (G_OBJECT (plugin), "about",
                    G_CALLBACK (nameday_about), NULL);
                    
   gtk_widget_show_all (GTK_WIDGET (plugin));                 
}

static void onEasters(GtkWidget *widget, gpointer data)
{
  GtkWidget *content_area,*dialog,*hbox,
  *stock,*table,*local_entry1,*label_year;
  
  gint response;
  
  g_autoptr(GDate) date = g_date_new();
  g_date_set_time_t (date, time (NULL));

  dialog = gtk_dialog_new_with_buttons (_("Date of Easter"),
					NULL,
					GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
					_("OK"),
					GTK_RESPONSE_OK,
          _("Cancel"),
          GTK_RESPONSE_CANCEL,
					NULL);

  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);
  gtk_box_pack_start (GTK_BOX (content_area), hbox, FALSE, FALSE, 0);

  stock = gtk_image_new_from_stock ("dialog-question", GTK_ICON_SIZE_DIALOG);
  gtk_box_pack_start (GTK_BOX (hbox), stock, FALSE, FALSE, 0);

  table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 4);
  gtk_table_set_col_spacings (GTK_TABLE (table), 4);
  gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 0);
  label_year = gtk_label_new_with_mnemonic (_("_year ?"));
  gtk_table_attach_defaults (GTK_TABLE (table), label_year, 0, 1, 0, 1);
  
  local_entry1 = gtk_spin_button_new_with_range(1583,2499,1);
  gtk_table_attach_defaults (GTK_TABLE (table), local_entry1, 1, 2, 0, 1);

  gtk_widget_show_all (hbox);
  response = gtk_dialog_run (GTK_DIALOG (dialog));

  if (response == GTK_RESPONSE_OK)
  {
    g_autofree gchar *tmp = getEaster(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(local_entry1)));
    showDialog(tmp);
    gtk_widget_destroy (dialog);
  }
}

static gboolean on_match_select(GtkEntryCompletion *widget,
  GtkTreeModel       *model,
  GtkTreeIter        *iter,
  gpointer            user_data)
{  
  GValue value = G_VALUE_INIT; 
  gtk_tree_model_get_value(model, iter, 0, &value);
  g_value_unset(&value);
  return FALSE;
} 

static gint nmd_compare(GtkTreeModel *model, GtkTreeIter *a , GtkTreeIter *b, gpointer data)
{
	g_autofree gchar *name1, *name2;
	gint ret = 0;
	
	gtk_tree_model_get(model, a, 0, &name1, -1);
    gtk_tree_model_get(model, b, 0, &name2, -1);
	
	if (name1 == NULL || name2 == NULL)
    {
       if (name1 == NULL && name2 == NULL)
            return 0; /* both equal => ret = 0 */
       ret = (name1 == NULL) ? -1 : 1;
    }
    else
    {
       ret = strcmp(name1,name2);
    }

	return ret;	
}

static void functor(gpointer data,gpointer udata)
{
	GtkTreeIter iter;
	GtkListStore *model = (GtkListStore*)udata;
	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter, 0, (gchar *)data, -1); 
}   

static void onSearchNmD(GtkWidget *widget, gpointer data)
{
  NamedaysPlugin *nmday = (NamedaysPlugin *)data;
  GtkWidget *content_area,*dialog,*hbox,*stock,
  *table,*local_entry1,*label_nameday;
  gint response;
  
  GtkEntryCompletion *completion;
  GtkListStore *model;
  GtkTreeIter iter; 

  g_autoptr(GDate) date = g_date_new();
  g_date_set_time_t (date, time (NULL));

  dialog = gtk_dialog_new_with_buttons (_("Date of Easter"),
					NULL,
					GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,
					_("OK"),
					GTK_RESPONSE_OK,
          _("Cancel"),
          GTK_RESPONSE_CANCEL,
					NULL);

  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

  hbox = gtk_hbox_new (TRUE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);
  gtk_box_pack_start (GTK_BOX (content_area), hbox, FALSE, FALSE, 0);

  stock = gtk_image_new_from_stock ("dialog-question", GTK_ICON_SIZE_DIALOG);
  gtk_box_pack_start (GTK_BOX (hbox), stock, FALSE, FALSE, 0);

  table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 4);
  gtk_table_set_col_spacings (GTK_TABLE (table), 4);
  gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 0);
  label_nameday = gtk_label_new_with_mnemonic (_("_Nameday..."));
  gtk_table_attach_defaults (GTK_TABLE (table),
			     label_nameday,
			     0, 1, 0, 1);
  local_entry1 =   gtk_entry_new ();
  /* set up completion */
  completion = gtk_entry_completion_new();

  gtk_entry_completion_set_text_column(completion, 0);
  gtk_entry_set_completion(GTK_ENTRY(local_entry1), completion);
  g_signal_connect(G_OBJECT (completion), "match-selected",
                G_CALLBACK (on_match_select), NULL); 
   model = gtk_list_store_new(1, G_TYPE_STRING);
   gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(model), 0, (GtkTreeIterCompareFunc)nmd_compare,nmday,NULL); 
  	
	if(nmday->namedays_list)
	{ 
		g_queue_foreach(nmday->namedays_list,(GFunc)functor,(gpointer)model);
	}
	gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(model));
                
  
  gtk_table_attach_defaults (GTK_TABLE (table), local_entry1, 1, 2, 0, 1);
  
  gtk_widget_show_all (hbox);
  response = gtk_dialog_run (GTK_DIALOG (dialog));

  if (response == GTK_RESPONSE_OK)
  {
		g_autofree gchar *tmp = getNamedayDay((gchar *)gtk_entry_get_text(GTK_ENTRY(local_entry1)),(NamedaysPlugin*)data);
		if(G_LIKELY(tmp!= NULL))
		{
			showDialog(tmp);
			gtk_widget_destroy (dialog);
		}	
  }
}

void onCopy(GtkWidget *widget, gpointer data)
{
	NamedaysPlugin *nm = (NamedaysPlugin *)data;
	const gchar *text = gtk_label_get_text(GTK_LABEL(nm->label));
	g_autofree gchar* _text = NULL;
	sprintf(_text,_("Namedays have %s"),text);
	gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), _text, strlen(_text));
	
}

void onCalendar(GtkWidget *widget, gpointer data)
{
	NamedaysPlugin *nm = (NamedaysPlugin *)data;	
	initDialogWithCalandar(nm->plugin, nm);
}

void showDialog(gchar * tmp)
{
	GtkWidget *dial = gtk_message_dialog_new(NULL,
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_INFO,
						GTK_BUTTONS_OK,
						"%s",
						tmp);
							
	gtk_dialog_run(GTK_DIALOG(dial));
	gtk_widget_destroy (dial);

}

static gboolean on_tooltip_cb (GtkWidget        *widget,
					gint              x,
					gint              y,
					gboolean          keyboard_mode,
					GtkTooltip       *tooltip,
					NamedaysPlugin *nmday)
{
  gchar mar_text[1000];	
  g_autoptr(GDate) date;
  time_t times = time(NULL);	
  /* Actual Date and x after */
  gint count = nmday->setcount;
  date = g_date_new();
  g_date_set_time_t (date, times);
  
  if(G_UNLIKELY(!date))
  {
		gtk_tooltip_set_text (tooltip, _("Cannot update date"));
  }
  else {
	gchar *markup_text = NULL;  
	strcpy(mar_text,"");
	for(int i = 0;i < count;i++)
	{
	  g_date_add_days(date,i);	
	  gchar *text = load_nm(date,nmday);
	  if(text) {
			strcat(mar_text, text);
			strcat(mar_text, "\n");
		}
	}	  
	markup_text = g_markup_escape_text(mar_text,-1);
	  
  gtk_tooltip_set_markup (tooltip, markup_text);
  }
  return TRUE;
}

static gchar* 
getEaster(int year)
{
	int A,B,C,D,E,F,G,H,I,K,L,M,month,day;
	if ((year<1583) || (year>2498)) {
		return g_strdup_printf (_("Wrong set year: %d \n Program can work with years  betwen 1583 and 2498.\n"),year);
	}

  A = year%19;
  B = year/100;
  C = year%100;
  D = B/4;
  E = B%4;
  F = (B+8)/25;
  G = (B-F+1)/3;
  H = ((19*A)+B-D-G+15)%30;
  I = C/4;
  K = C%4;
  L = (32+(2*E)+(2*I)-H-K)%7;
  M = (A+(11*H)+(22*L))/451;
  month = (H+L-(7*M)+114)/31;
  day = ( (H+L-(7*M)+114)%31)+1;
  return g_strdup_printf(_("Easter sunday in %d is %d. %d "), year, day, month);
}

static gchar * getNamedayDay(gchar *sname, NamedaysPlugin *nmday)
{
	return g_strdup_printf(_("Namedays for name %s is at %s"), sname, findNamedaysInFile(nmday->setting1, sname));
}
