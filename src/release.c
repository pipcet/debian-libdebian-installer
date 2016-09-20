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

#define _GNU_SOURCE

#include <config.h>

#include <debian-installer/release.h>
#include "release_internal.h"

#include <debian-installer/mem.h>

#include <stdio.h>
#include <string.h>

static di_compare_func files_compare;

static int files_compare(const void *key1, const void *key2)
{
  const char *file1 = key1, *file2 = key2;
  return strcmp(file1, file2);
}

static void files_destroy(void *data)
{
  struct di_release_file_internal *file = data;
  di_free(file->name);
  di_free(file->name_sha256);
  di_free(file->sum_sha256);
  di_free(file);
}

static void files_finish(void *key __attribute__((unused)), void *value, void *user_data)
{
  struct di_release *release = user_data;
  struct di_release_file_internal *file = value;

  if (release->acquire_byhash)
  {
    const char *delim = strrchr(file->name, '/');
    if (delim)
      if (asprintf(&file->name_sha256, "%.*s/by-hash/SHA256/%s", (int)(delim - file->name), file->name, file->sum_sha256) < 0)
        file->name_sha256 = NULL;
  }
}

di_release *di_release_read(FILE *f)
{
  di_release *ret = di_new0(struct di_release, 1);
  ret->files = di_tree_new_full(files_compare, NULL, files_destroy);

  if (di_release_parser(f, ret) < 0)
  {
    di_release_free(ret);
    return NULL;
  }

  di_tree_foreach(ret->files, files_finish, ret);

  return ret;
}

void di_release_free(di_release *release)
{
  di_free(release->codename);
  di_free(release->description);
  di_free(release->label);
  di_free(release->origin);
  di_free(release->suite);
  di_free(release->version);
  di_tree_destroy(release->files);
  di_free(release);
}

const char *di_release_get_codename(const di_release *release)
{
  return release ? release->codename : NULL;
}

const char *di_release_get_description(const di_release *release)
{
  return release ? release->description : NULL;
}

const char *di_release_get_label(const di_release *release)
{
  return release ? release->label : NULL;
}

const char *di_release_get_origin(const di_release *release)
{
  return release ? release->origin : NULL;
}

const char *di_release_get_suite(const di_release *release)
{
  return release ? release->suite : NULL;
}

const char *di_release_get_version(const di_release *release)
{
  return release ? release->version : NULL;
}

bool di_release_get_file(di_release_file *out, const di_release *release, const char *filename)
{
  if (!release)
    return false;

  struct di_release_file_internal *f = di_tree_lookup(release->files, filename);

  if (!f)
    return false;

  out->name = f->name;
  out->name_byhash = f->name_sha256;
  out->checksum.type = DI_RELEASE_FILE_CHECKSUM_SHA256;
  out->checksum.value = f->sum_sha256;

  return true;
}
