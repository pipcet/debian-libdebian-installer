/*
 * release_parser.c - Parser for Release files
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

#include <debian-installer/release.h>
#include "release_internal.h"

#include <debian-installer/file_rfc822.h>
#include <debian-installer/mem.h>
#include <debian-installer/string.h>

static di_file_fields_function_read
  di_release_parser_read_file;

const di_file_fieldinfo
  di_release_parser_field_acquire_byhash =
    DI_FILE_FIELDINFO
    (
      "Acquire-By-Hash",
      di_file_read_boolean,
      NULL,
      offsetof(di_release, acquire_byhash)
    ),
  di_release_parser_field_codename =
    DI_FILE_FIELDINFO
    (
      "Codename",
      di_file_read_string,
      NULL,
      offsetof(di_release, codename)
    ),
  di_release_parser_field_description =
    DI_FILE_FIELDINFO
    (
      "Description",
      di_file_read_string,
      NULL,
      offsetof(di_release, description)
    ),
  di_release_parser_field_label =
    DI_FILE_FIELDINFO
    (
      "Label",
      di_file_read_string,
      NULL,
      offsetof(di_release, label)
    ),
  di_release_parser_field_origin =
    DI_FILE_FIELDINFO
    (
      "Origin",
      di_file_read_string,
      NULL,
      offsetof(di_release, origin)
    ),
  di_release_parser_field_suite =
    DI_FILE_FIELDINFO
    (
      "Suite",
      di_file_read_string,
      NULL,
      offsetof(di_release, suite)
    ),
  di_release_parser_field_version =
    DI_FILE_FIELDINFO
    (
      "Version",
      di_file_read_string,
      NULL,
      offsetof(di_release, version)
    ),
  di_release_parser_field_sha256 =
    DI_FILE_FIELDINFO
    (
      "SHA256",
      di_release_parser_read_file,
      NULL,
      DI_RELEASE_FILE_CHECKSUM_SHA256
    );

/**
 * Standard Release file
 */
const di_file_fieldinfo *di_release_parser_fieldinfo[] =
{
  &di_release_parser_field_acquire_byhash,
  &di_release_parser_field_codename,
  &di_release_parser_field_description,
  &di_release_parser_field_label,
  &di_release_parser_field_origin,
  &di_release_parser_field_suite,
  &di_release_parser_field_version,
  &di_release_parser_field_sha256,
  NULL
};

__attribute__((visibility("internal")))
int di_release_file_read(FILE *f, di_release *release)
{
  di_file_info *info = di_file_info_alloc();
  di_file_info_add(info, di_release_parser_fieldinfo);

  int ret = di_file_rfc822_read_one(f, info, release);

  di_file_info_free(info);

  return ret;
}

static void di_release_parser_read_file(data, fip, field, value, user_data)
  void **data;
  const di_file_fieldinfo *fip __attribute__((unused));
  di_rstring *field __attribute__((unused));
  di_rstring *value;
  void *user_data __attribute__((unused));
{
  char *begin = value->string, *end = value->string + value->size;
  di_release *release = *data;

  while (1)
  {
    char *next = memchr(begin, '\n', end - begin);
    if (!next)
      next = end;
    *next = '\0';

    char *sum, *filename;
    size_t size;

    int ret = sscanf(begin, "%ms%*[ ]%zu%*[ ]%ms", &sum, &size, &filename);

    if (ret == 3)
    {
      struct di_release_file_internal *f = di_new0(struct di_release_file_internal, 1);
      f->name = filename;
      f->sum_sha256 = sum;
      di_tree_insert(release->files, filename, f);
    }

    begin = next + 1;
    if (begin >= end)
      break;
  }
}
