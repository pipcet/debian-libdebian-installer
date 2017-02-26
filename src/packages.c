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

#include <string.h>

#include "packages_internal.h"

#include <debian-installer/mem.h>

static di_compare_func package_cmp;
static int package_cmp(const void *key1, const void *key2)
{
  const struct di_packages_entry *p1 = key1, *p2 = key2;
  return strcmp(p1->p.package, p2->p.package);
}

static di_destroy_notify package_destroy;
static void package_destroy(void *data)
{
  struct di_packages_entry *p = data;
  di_package_destroy(&p->p);
  di_free(p);
}

static di_file_read_entry_new package_new;
static void *package_new(void *user_data)
{
  return di_new0(struct di_packages_entry, 1);
}

static di_file_read_entry_finish package_finish;
static int package_finish(void *data, void *user_data)
{
  di_packages_add_package(user_data, data);
}

di_packages *di_packages_new()
{
  di_packages *ret = di_new0(struct di_packages, 1);

  ret->p = di_tree_new_full(package_cmp, NULL, package_destroy);

  return ret;
}

int di_packages_read(di_packages *packages, FILE *f)
{
  return di_package_file_read_many(f, package_new, package_finish, packages);
}

void di_packages_free(di_packages *packages)
{
  di_tree_destroy(packages->p);
  di_free(packages);
}

static const struct di_packages_entry *get_entry(di_packages *packages, const char *name)
{
  if (!packages || !name)
    return NULL;
  const struct di_packages_entry lookup = { .p = { .package = (char *)name } };
  return di_tree_lookup(packages->p, &lookup);
}

void di_packages_add_package(di_packages *packages, di_package *p)
{
  if (!get_entry(packages, di_package_get_package(p)))
  {
    di_tree_insert(packages->p, p, p);
  }
}

const di_package *di_packages_get_package(di_packages *packages, const char *name)
{
  const struct di_packages_entry *e = get_entry(packages, name);
  return e ? &e->p : NULL;
}
