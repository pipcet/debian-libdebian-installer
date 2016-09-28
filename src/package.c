/*
 * package.c - Debian package support
 *
 * Copyright (C) 2016 Bastian Blank <waldi@debian.org>
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
include <debian-installer/package.h>

di_package *di_package_read(FILE *f)
{
  return NULL;
}

void di_package_free(di_package *package)
{
}

const char *di_package_get_package(const di_package *package)
{
  return NULL;
}

enum di_package_priority di_package_get_priority(const di_package *package)
{
  return 0;
}

const char *di_package_get_section(const di_package *package)
{
  return NULL;
}

const char *di_package_get_architecture(const di_package *package)
{
  return NULL;
}

const di_version *di_package_get_version(const di_package *package)
{
  return NULL;
}

const char *di_package_get_relation(const di_package *package, enum di_package_relation_type type)
{
  return NULL;
}

const char *di_package_get_filename(const di_package *package)
{
  return NULL;
}

const int di_package_get_size(const di_package *package)
{
  return 0;
}

const char *di_package_get_description(const di_package *package)
{
  return NULL;
}

int di_package_get_di_installermenuitem(const di_package *package)
{
  return 0;
}

const char *di_package_get_di_kernelversion(const di_package *package)
{
  return NULL;
}

const char *di_package_get_di_subarchitecture(const di_package *package)
{
  return NULL;
}
