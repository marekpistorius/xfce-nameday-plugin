/* 
* (C) 2024-2026 Marek Pistorius
* This file is part of xfce-nameday-plugin.
* xfce-nameday-plugin is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the[...]
*
* xfce-nameday-plugin is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU[...]
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
#include "nameday-plugin-data.h"

/*
 * create_list_namedays:
 * - Reads the file fully and splits it by lines.
 * - For each line takes the text after the last '|' (if present),
 *   trims it and appends to a GList (owned strings).
 * - Returns the list (NULL if file can't be read or no entries).
 * - Caller must free the list and each string (g_list_free_full(list, g_free)).
 */
GList* create_list_namedays(const gchar* _file)
{
	gchar *contents = NULL;
	gsize length = 0;

	if (!_file)
		return NULL;

	if (!g_file_get_contents(_file, &contents, &length, NULL))
		return NULL;

	GList *list = NULL;
	gchar **lines = g_strsplit(contents, "\n", 0);

	for (gint i = 0; lines[i] != NULL; ++i)
	{
		gchar *line = lines[i];
		if (*line == '\0')
			continue;

		char *tmp = strrchr(line, '|');
		if (!tmp)
			continue;

		/* duplicate and strip whitespace/newline */
		gchar *name = g_strdup(tmp + 1);
		g_strstrip(name);
		if (*name != '\0')
			list = g_list_append(list, name);
		else
			g_free(name);
	}

	g_strfreev(lines);
	g_free(contents);
	return list;
}

/*
 * findNamedaysInFile:
 * - Searches for a line where the token after the first '|' matches 'name' (case-sensitive).
 * - If found, returns a newly-allocated string containing everything BEFORE the last '|'
 *   (trimmed). Caller must free the returned string.
 * - Returns NULL if not found or on error.
 */
gchar *findNamedaysInFile(gchar *_file, gchar *name)
{
	gchar *contents = NULL;
	gsize length = 0;

	if (!_file || !name)
		return NULL;

	if (!g_file_get_contents(_file, &contents, &length, NULL))
		return NULL;

	gchar *result = NULL;
	gchar **lines = g_strsplit(contents, "\n", 0);

	for (gint i = 0; lines[i] != NULL; ++i)
	{
		gchar *line = lines[i];
		if (*line == '\0')
			continue;

		/* first '|' separates left part and the token we compare with 'name' */
		char *first_pipe = strchr(line, '|');
		if (!first_pipe)
			continue;

		gchar *token = g_strdup(first_pipe + 1);
		g_strstrip(token);

		if (g_strcmp0(token, name) == 0)
		{
			/* take everything before the last '|' (if present) */
			char *last_pipe = strrchr(line, '|');
			if (last_pipe)
			{
				gsize len_before = (gsize)(last_pipe - line);
				/* duplicate and strip */
				result = g_strndup(line, len_before);
				g_strstrip(result);
			}
			g_free(token);
			break;
		}
		g_free(token);
	}

	g_strfreev(lines);
	g_free(contents);
	return result; /* may be NULL if not found */
}

/*
 * NamedayInFebruaryLeapYear:
 * - Reads first (non-empty) line from file and returns it trimmed.
 * - Caller must free the returned string.
 */
gchar *NamedayInFebruaryLeapYear(gchar *file)
{
	if (!file)
		return NULL;

	gchar *contents = NULL;
	gsize length = 0;

	if (!g_file_get_contents(file, &contents, &length, NULL))
		return NULL;

	gchar *result = NULL;
	gchar **lines = g_strsplit(contents, "\n", 0);

	/* find first non-empty line */
	for (gint i = 0; lines[i] != NULL; ++i)
	{
		if (lines[i][0] == '\0')
			continue;

		result = g_strdup(lines[i]);
		g_strstrip(result);
		break;
	}

	g_strfreev(lines);
	g_free(contents);
	return result;
}
