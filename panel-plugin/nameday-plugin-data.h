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
 
#ifndef __NAMEDAY_DATA_H__
#define __NAMEDAY_DATA_H__

#include <glib.h>

G_BEGIN_DECLS

GList* create_list_namedays();

int getNamedaysForFile(gchar *_file, char *dData[365]);

gchar* getNamedayDayForDayMonth(gchar* _file, guint day, guint month);

gboolean getNamedaysForFileInGList(gchar *_file, GList *namedays);

gchar *findNamedaysInFile(gchar *_file, gchar *name);

gchar *NamedayInFebruaryLeapYear(gchar *file);

G_END_DECLS
#endif
