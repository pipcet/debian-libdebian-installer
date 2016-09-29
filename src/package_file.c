/*
 * package_parser.c
 *
 * Copyright (C) 2003 Bastian Blank <waldi@debian.org>
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

#include "package_internal.h"

#include <debian-installer/file_rfc822.h>
#include <debian-installer/string.h>
#include <debian-installer/version.h>

#include <stddef.h>

static di_file_fields_function_read
  di_package_parser_read_priority,
  di_package_parser_read_version;

static const di_file_fieldinfo 
  di_package_parser_field_package = 
    DI_FILE_FIELDINFO
    (
      "Package",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, package)
    ),
  di_package_parser_field_priority =
    DI_FILE_FIELDINFO
    (
      "Priority",
      di_package_parser_read_priority,
      NULL,
      0
    ),
  di_package_parser_field_section =
    DI_FILE_FIELDINFO
    (
      "Section",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, section)
    ),
  di_package_parser_field_architecture =
    DI_FILE_FIELDINFO
    (
      "Architecture",
      di_file_read_string,
      di_file_write_string,
      offsetof (di_package, architecture)
    ),
  di_package_parser_field_version =
    DI_FILE_FIELDINFO
    (
      "Version",
      di_package_parser_read_version,
      NULL,
      0
    ),
  di_package_parser_field_replaces =
    DI_FILE_FIELDINFO
    (
      "Replaces",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, relation_replaces)
    ),
  di_package_parser_field_provides =
    DI_FILE_FIELDINFO
    (
      "Provides",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, relation_provides)
    ),
  di_package_parser_field_depends =
    DI_FILE_FIELDINFO
    (
      "Depends",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, relation_depends)
    ),
  di_package_parser_field_pre_depends =
    DI_FILE_FIELDINFO
    (
      "Pre-Depends",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, relation_predepends)
    ),
  di_package_parser_field_recommends =
    DI_FILE_FIELDINFO
    (
      "Recommends",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, relation_recommends)
    ),
  di_package_parser_field_suggests =
    DI_FILE_FIELDINFO
    (
      "Suggests",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, relation_suggests)
    ),
  di_package_parser_field_conflicts =
    DI_FILE_FIELDINFO
    (
      "Conflicts",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, relation_conflicts)
    ),
  di_package_parser_field_enhances =
    DI_FILE_FIELDINFO
    (
      "Enhances",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, relation_enhances)
    ),
  di_package_parser_field_filename =
    DI_FILE_FIELDINFO
    (
      "Filename",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, filename)
    ),
  di_package_parser_field_size =
    DI_FILE_FIELDINFO
    (
      "Size",
      di_file_read_int,
      di_file_write_int,
      offsetof(di_package, size)
    ),
  di_package_parser_field_description =
    DI_FILE_FIELDINFO
    (
      "Description",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, description)
    ),
  di_package_parser_field_di_installermenuitem =
    DI_FILE_FIELDINFO
    (
      "Installer-Menu-Item",
      di_file_read_int,
      di_file_write_int,
      offsetof(di_package, di_installermenuitem)
    ),
  di_package_parser_field_di_kernelversion =
    DI_FILE_FIELDINFO
    (
      "Kernel-Version",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, di_kernelversion)
    ),
  di_package_parser_field_di_subarchitecture =
    DI_FILE_FIELDINFO
    (
      "Subarchitecture",
      di_file_read_string,
      di_file_write_string,
      offsetof(di_package, di_subarchitecture)
    );

static const di_file_fieldinfo *di_package_parser_fieldinfo[] =
{
  &di_package_parser_field_package,
  &di_package_parser_field_priority,
  &di_package_parser_field_section,
  &di_package_parser_field_architecture,
  &di_package_parser_field_version,
  &di_package_parser_field_filename,
  &di_package_parser_field_size,
  &di_package_parser_field_description,
  &di_package_parser_field_replaces,
  &di_package_parser_field_provides,
  &di_package_parser_field_depends,
  &di_package_parser_field_pre_depends,
  &di_package_parser_field_recommends,
  &di_package_parser_field_suggests,
  &di_package_parser_field_conflicts,
  &di_package_parser_field_enhances,
  &di_package_parser_field_di_installermenuitem,
  &di_package_parser_field_di_kernelversion,
  &di_package_parser_field_di_subarchitecture,
  NULL
};

__attribute__((visibility("internal")))
int di_package_file_read(FILE *f, di_package *package)
{
  di_file_info *info = di_file_info_alloc();
  di_file_info_add(info, di_package_parser_fieldinfo);

  int ret = di_file_rfc822_read_one(f, info, package);

  di_file_info_free(info);

  return ret;
}

static int di_package_array_text_from(const char *const *array, const char *text)
{
  for (int i = 1; array[i]; i++)
    if (strcmp(array[i], text) == 0)
      return i;
  return 0;
}

static const char *const di_package_priority_text[] =
{
  "unspecified",
  "extra",                              /* == di_package_priority_extra */
  "optional",                           /* == di_package_priority_optional */
  "standard",                           /* == di_package_priority_standard */
  "important",                          /* == di_package_priority_important */
  "required",                           /* == di_package_priority_required */
  NULL
};

static void di_package_parser_read_priority(
  void **data,
  const di_file_fieldinfo *fip __attribute__((unused)),
  di_rstring *field __attribute__((unused)),
  di_rstring *value,
  void *user_data __attribute__((unused)))
{
  di_package *p = *data;
  p->priority = di_package_array_text_from(di_package_priority_text, value->string);
}

static void di_package_parser_read_version(
  void **data,
  const di_file_fieldinfo *fip __attribute__((unused)),
  di_rstring *field __attribute__((unused)),
  di_rstring *value,
  void *user_data __attribute__((unused)))
{
  di_package *p = *data;
  p->version = di_version_parse(value->string);
}
