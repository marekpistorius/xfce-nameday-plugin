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

gchar* g_substr (const gchar* string,
          gint         start,
          gint         end)
{
  gsize len = (end - start + 1);
  gchar *output = g_malloc0 (len + 1);
  return g_utf8_strncpy (output, &string[start], len);
}

/* Just Search files in path */
GList *getdirlist(gchar *path)
{
  GDir *dir = g_dir_open(path,0,NULL);
  GList *lists = NULL;
  const gchar * files;
  gchar *ext = NULL,*leap = NULL;

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

 return lists;
}
