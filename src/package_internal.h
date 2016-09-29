/*
 * package_internal.h
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

#include <debian-installer/package.h>

struct di_package {
  char *package;
  enum di_package_priority priority;
  char *section;
  char *architecture;
  di_version *version;
  char *relation_replaces;
  char *relation_provides;
  char *relation_depends;
  char *relation_predepends;
  char *relation_recommends;
  char *relation_suggests;
  char *relation_conflicts;
  char *relation_enhances;
  char *filename;
  int size;
  char *description;

  int di_installermenuitem;
  char *di_kernelversion;
  char *di_subarchitecture;
};

int di_package_parser(FILE *, di_package *release);
