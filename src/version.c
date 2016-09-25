/*
 * version.c - Debian version support
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

#include <debian-installer/version.h>

#include <debian-installer/mem.h>
#include <debian-installer/string.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

di_version *di_version_parse(const char *version)
{
  di_version *ret = di_new0(di_version, 1);

  const char *end = version + strlen(version);
  const char *colon = strchr(version, ':');

  if (colon) {
    char *eepochcolon;
    ret->epoch = strtoul(version, &eepochcolon, 10);
    if (colon != eepochcolon || !*++colon)
      goto error;

    version = colon;
  }

  const char *hyphen = strrchr(version, '-');
  if (hyphen)
  {
    ret->upstream = di_stradup(version, hyphen - version);
    ret->revision = di_stradup(hyphen + 1, end - hyphen - 1);
  }
  else
    ret->upstream = di_stradup(version, end - version);

  return ret;

error:
  di_version_free(ret);
  return NULL;
}

void di_version_free(di_version *version)
{
  di_free(version->upstream);
  di_free(version->revision);
  di_free(version);
}

#define order(x) ((x) == '~' ? -1 \
        : isdigit((x)) ? 0 \
        : !(x) ? 0 \
        : isalpha((x)) ? (x) \
        : (x) + 256)

static int verrevcmp(const char *val, const char *ref)
{
  if (!val)
    val = "";
  if (!ref)
    ref = "";

  while (*val || *ref) {
    int first_diff = 0;

    while ((*val && !isdigit (*val)) || (*ref && !isdigit (*ref))) {
      int vc = order(*val), rc = order(*ref);
      if (vc != rc)
        return vc - rc;
      val++;
      ref++;
    }

    while (*val == '0')
      val++;
    while (*ref == '0')
      ref++;
    while (isdigit(*val) && isdigit(*ref)) {
      if (!first_diff)
        first_diff = *val - *ref;
      val++;
      ref++;
    }
    if (isdigit(*val))
      return 1;
    if (isdigit(*ref))
      return -1;
    if (first_diff)
      return first_diff;
  }
  return 0;
}

int di_version_cmp(const di_version *const a, const di_version *const b)
{
  if (a->epoch > b->epoch)
    return 1;
  if (a->epoch < b->epoch)
    return -1;

  int r = verrevcmp(a->upstream, b->upstream);
  if (r != 0)
    return r;

  return verrevcmp(a->revision, b->revision);
}
