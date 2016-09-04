/*
 * release.c
 *
 * Copyright (C) 2003-2016 Bastian Blank <waldi@debian.org>
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

#include <debian-installer/release.h>

di_release *di_release_read(const char *mem, size_t len)
{
  return NULL;
}

di_release *di_release_read_file(const char *filename)
{
  return NULL;
}

void di_release_free(di_release *release)
{
  return;
}

const char *di_release_get_codename(const di_release *release)
{
  return NULL;
}

const char *di_release_get_description(const di_release *release)
{
  return NULL;
}

const char *di_release_get_label(const di_release *release)
{
  return NULL;
}

const char *di_release_get_origin(const di_release *release)
{
  return NULL;
}

const char *di_release_get_suite(const di_release *release)
{
  return NULL;
}

const char *di_release_get_version(const di_release *release)
{
  return NULL;
}

di_release_file di_release_get_file(const di_release *release, const char *filename)
{
  di_release_file file = { 0, };
  return file;
}
