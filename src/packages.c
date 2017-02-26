/*
 * packages.c - Debian Packages support
 *
 * Copyright (C) 2017 Bastian Blank <waldi@debian.org>
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>

#include "packages_internal.h"

di_packages *di_packages_new()
{
  return NULL;
}

int di_packages_read(di_packages *packages, FILE *f)
{
  return 0;
}

void di_packages_free(di_packages *packages)
{
}

void di_packages_add_package(di_packages *packages, di_package *p)
{
}

const di_package *di_packages_get_package(di_packages *packages, const char *name)
{
  return NULL;
}
