/*
 * release.h
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

#ifndef DEBIAN_INSTALLER__RELEASE_H
#define DEBIAN_INSTALLER__RELEASE_H

#include <stddef.h>

typedef struct di_release di_release;
typedef struct di_release_file di_release_file;

/**
 * @addtogroup di_release
 * @{
 */

struct di_release_file {
  const char *name;
  const char *name_byhash;

  struct di_release_file_checksum {
    enum {
      DI_RELEASE_FILE_CHECKSUM_SHA256 = 1,
    } type;
    const char *value;
  } checksum;
};

di_release *di_release_read(const char *mem, size_t len);
di_release *di_release_read_file(const char *filename);
void di_release_free(di_release *);

const char *di_release_get_codename(const di_release *);
const char *di_release_get_description(const di_release *);
const char *di_release_get_label(const di_release *);
const char *di_release_get_origin(const di_release *);
const char *di_release_get_suite(const di_release *);
const char *di_release_get_version(const di_release *);

di_release_file di_release_get_file(const di_release *, const char *filename);

/** @} */
#endif
