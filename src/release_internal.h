/*
 * release_internal.h
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

#include <debian-installer/tree.h>

struct di_release {
  bool acquire_byhash;

  char *description;
  char *origin;
  char *label;
  char *version;
  char *suite;
  char *codename;

  di_tree *files;
};

struct di_release_file_internal {
  char *name;
  char *name_sha256;
  char *sum_sha256;
};

int di_release_file_read(FILE *, di_release *release);
