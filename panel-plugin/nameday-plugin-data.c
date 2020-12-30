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
#ifdef HAVE_STRING_H
	#include <string.h>
#endif

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/xfce-panel-plugin.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "nameday-utils.h"
#include "nameday-plugin-data.h"

GList* create_list_namedays(const gchar* _file)
{

	FILE *file;
	char sData[256];
	GList* list;

	file = g_fopen(_file, "r");

	 if( file == NULL)
		  return list;

	 while (fgets(sData,256,file) != NULL)
	 {
		char *tmp = strrchr( sData, '|');
		g_list_append(list, g_strdup(tmp+1));
	 }
	fclose(file);
	return list;
}

gchar *findNamedaysInFile(gchar *_file, gchar *name)
{
	FILE *file;
	char sData[256];
	file = g_fopen(_file, "r");

	if( file == NULL)
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

gchar *NamedayInFebruaryLeapYear(gchar *file)
{
	FILE *f = NULL;
	gchar name[366];
	f = g_fopen(file, "r");

	if(f == NULL)
		return NULL;
	if (fgets(name,366,f)!= NULL)
	{
		fclose(f);
		return g_strdup(name);
	}
	fclose(f);
	return NULL;
}
