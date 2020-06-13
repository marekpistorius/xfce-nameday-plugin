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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif
#ifdef HAVE_STRING_H
	#include <string.h>
#endif

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4panel/xfce-hvbox.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "nameday-plugin-data.h"

int getNamedaysForFile(gchar *_file, char *dData[365])
{
	FILE *file;
	char sData[256];
	int i = 0;
	file = g_fopen(_file, "r");

	if(!file)
		return 1;

	while(fgets(sData,256,file) != NULL)
	{
		char *tmp = strchr(sData, '|');
		if(tmp && (i < 365))
		   dData[i++]= g_strdup(g_strstrip(g_strdup(tmp+1)));
		else continue;
	}
	fclose(file);
	return 0;
}

gchar * g_substr (const gchar* string,
          gint         start,
          gint         end)
{
  gsize len = (end - start + 1);
  gchar *output = g_malloc0 (len + 1);
  return g_utf8_strncpy (output, &string[start], len);
}


gchar *findNamedaysInFile(gchar *_file, gchar *name)
{
	FILE *file;
	char sData[256];
	file = g_fopen(_file, "r");

	if(!file)
		  return NULL;

	 while (fgets(sData,256,file) != NULL)
	 {
			char *tmp = strchr(sData, '|');
			char *tmp2;
		  	char *stmp = g_strdup(g_strstrip(g_strdup(tmp+1)));

		  	if(g_strcmp0 (stmp,name) == 0)
			{
				char *dat;
				tmp2 = strrchr(sData, '|');
				gint end = tmp2-sData+1;
				dat = g_substr(sData,0,end-2);
				fclose(file);
				return g_strdup(dat);
			}
	 }
	 fclose(file);
	 return NULL;
}

gboolean getNamedaysForFileInGList(gchar *_file, GList *namedays)
{
	FILE *file;
	char sData[256];
	GList *list = NULL;

	file = g_fopen(_file, "r");

	 if(!file)
		  return FALSE;

	 while (fgets(sData,256,file) != NULL)
	 {
		char *tmp = strrchr( sData, '|');
		list = g_list_prepend(list, g_strdup(tmp+1));
	 }

	namedays = g_list_copy(list);
	fclose(file);
	return TRUE;
}

gchar* getNamedayDayForDayMonth(gchar*_file, guint day, guint month)
{
	FILE *file;
	char sData[256];
	file = g_fopen(_file, "r");

	if(!file)
			return "None";

	while(fgets(sData,256,file) != NULL)
	{
		char *tmp = strrchr(sData, '|');
		if(tmp)
		{
			char *tmp2 = g_substr(sData,0,(int)tmp);
			char *tmp3 = strrchr(tmp2,'.');
			char *_day = g_substr(tmp2,0,(int)tmp3);
			int xday = atoi(_day);
			char *_month = g_substr(tmp3,0,(int)tmp);
			int xmonth = atoi(_month);

			if(xmonth == month && xday == day) {
					fclose(file);
					return g_substr(tmp,0,25);
			}
		}
	}
	fclose(file);
	return "Not Find";
}

/* Just Search files in path */
GList *getdirlist(gchar *path)
{
  GDir *dir = g_dir_open(path,0,NULL);
  GList *lists = NULL;
  const gchar * files;
  gchar *ext = NULL;
  gchar *leap = NULL;

  while ( ( files = g_dir_read_name( dir)) != NULL)
  {

		gchar *tmp = g_strjoin(NULL,path,files,NULL);
		{
			if( g_file_test( tmp, G_FILE_TEST_IS_REGULAR ) )
			{
				ext = g_strstr_len(tmp,-1,".dat");
				leap = g_strrstr(tmp,".leap");

				if(ext && !leap)
				{
					lists = g_list_prepend( lists, g_strdup( tmp ) );
				}
			}
		}
	}
  g_dir_close(dir);

 //end
 return lists;
}

gchar *NamedayInFebruaryLeapYear(gchar *file)
{
	FILE *f = NULL;
	gchar name[366];
	f = g_fopen(file, "r");

	if(!f)
		return NULL;
	if (fgets(name,366,f)!= NULL)
	{
		fclose(f);
		return g_strdup(name);
	}
	fclose(f);
	return NULL;
}
