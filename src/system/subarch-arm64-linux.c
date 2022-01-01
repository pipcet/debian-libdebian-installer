/*
 * subarch-arm64-linux.c
 *
 * Copyright (C) 2013 Ian Campbell <ijc@hellion.org.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <debian-installer/system/subarch.h>
#include <debian-installer/system/efi.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

struct map {
	char *entry;
	char *ret;
};

static struct map map_hardware[] = {
	{ "Apple ", "m1" },
	{ NULL, NULL },
};

static int read_dt_model(char *entry, int entry_len)
{
	FILE *model;
	int ret;

	model = fopen("/proc/device-tree/model", "r");
	if (model == NULL)
		return 1;

	ret = fgets(entry, entry_len, model) == NULL;
	fclose(model);
	return ret;
}

const char *di_system_subarch_analyze(void)
{
	if (di_system_is_efi())
		return "efi";
	else {
		char entry[256];
		int ret;
		int i;

		ret = read_dt_model(entry, sizeof(entry));

		if (!ret) {
			for (i = 0; map_hardware[i].entry; i++)
				if (!strncasecmp(map_hardware[i].entry, entry,
						 strlen(map_hardware[i].entry)))
					return map_hardware[i].ret;
		}
	}

	return "generic";
}
