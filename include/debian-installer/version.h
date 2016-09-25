/*
 * version.h - Debian version support
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

#ifndef DEBIAN_INSTALLER__VERSION_H
#define DEBIAN_INSTALLER__VERSION_H

typedef struct di_version di_version;

/**
 * @addtogroup di_version
 * @{
 */

/**
 * @brief Version
 */
struct di_version
{
  unsigned long epoch;                           /**< epoch */
  char *upstream;                                /**< upstream */
  char *revision;                                /**< debian revision */
};

di_version *di_version_parse(const char *version);
void di_version_free(di_version *version);

int di_version_cmp(const di_version *a, const di_version *b);

/** @} */
#endif
