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

#include "package_internal.h"

#include <debian-installer/mem.h>

#include <stdio.h>
#include <string.h>

di_package *di_package_read(FILE *f)
{
  di_package *ret = di_new0(struct di_package, 1);

  if (di_package_file_read(f, ret) < 0)
  {
    di_package_free(ret);
    return NULL;
  }

  return ret;
}

void di_package_destroy(di_package *package)
{
  di_free(package->package);
  di_free(package->section);
  di_free(package->architecture);
  di_version_free(package->version);
  di_free(package->relation_replaces);
  di_free(package->relation_provides);
  di_free(package->relation_depends);
  di_free(package->relation_predepends);
  di_free(package->relation_recommends);
  di_free(package->relation_suggests);
  di_free(package->relation_conflicts);
  di_free(package->relation_enhances);
  di_free(package->filename);
  di_free(package->description);
  di_free(package->di_kernelversion);
  di_free(package->di_subarchitecture);
}

void di_package_free(di_package *package)
{
  di_package_destroy(package);
  di_free(package);
}

const char *di_package_get_package(const di_package *package)
{
  return package ? package->package : NULL;
}

enum di_package_priority di_package_get_priority(const di_package *package)
{
  return package ? package->priority : 0;
}

const char *di_package_get_section(const di_package *package)
{
  return package ? package->section : NULL;
}

const char *di_package_get_architecture(const di_package *package)
{
  return package ? package->architecture : NULL;
}

const di_version *di_package_get_version(const di_package *package)
{
  return package ? package->version : NULL;
}

const char *di_package_get_relation(const di_package *package, enum di_package_relation_type type)
{
  switch (type)
  {
    case di_package_relation_type_replaces:
      return package ? package->relation_replaces : NULL;

    case di_package_relation_type_provides:
      return package ? package->relation_provides : NULL;

    case di_package_relation_type_depends:
      return package ? package->relation_depends : NULL;

    case di_package_relation_type_predepends:
      return package ? package->relation_predepends : NULL;

    case di_package_relation_type_recommends:
      return package ? package->relation_recommends : NULL;

    case di_package_relation_type_suggests:
      return package ? package->relation_suggests : NULL;

    case di_package_relation_type_conflicts:
      return package ? package->relation_conflicts : NULL;

    case di_package_relation_type_enhances:
      return package ? package->relation_enhances : NULL;
  }
  return NULL;
}

const char *di_package_get_filename(const di_package *package)
{
  return package ? package->filename : NULL;
}

const int di_package_get_size(const di_package *package)
{
  return package ? package->size : 0;
}

const char *di_package_get_description(const di_package *package)
{
  return package ? package->description : NULL;
}

const di_digest *di_package_get_digest(const di_package *package)
{
  return package ? &package->digest : NULL;
}

int di_package_get_di_installermenuitem(const di_package *package)
{
  return package ? package->di_installermenuitem : 0;
}

const char *di_package_get_di_kernelversion(const di_package *package)
{
  return package ? package->di_kernelversion : NULL;
}

const char *di_package_get_di_subarchitecture(const di_package *package)
{
  return package ? package->di_subarchitecture : NULL;
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

const char *const di_package_status_text[] =
{
  "undefined",                          /* == di_package_status_undefined */
  "not-installed",                      /* == di_package_status_not_installed */
  "unpacked",                           /* == di_package_status_unpacked */
  "installed",                          /* == di_package_status_installed */
  "half-configured",                    /* == di_package_status_half_configured */
  "config-files",                       /* == di_package_status_config_files */
  NULL
};

static int di_package_array_text_from(const char *const *array, const char *text)
{
  for (int i = 1; array[i]; i++)
    if (strcmp(array[i], text) == 0)
      return i;
  return 0;
}

enum di_package_priority di_package_priority_text_from(const char *priority)
{
  return di_package_array_text_from(di_package_priority_text, priority);
}

enum di_package_status di_package_status_text_from(const char *status)
{
  return di_package_array_text_from(di_package_status_text, status);
}
