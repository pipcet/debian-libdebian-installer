/*
 * dpkg.c
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * $Id: dpkg.c,v 1.3 2003/09/24 11:49:52 waldi Exp $
 */

#include <config.h>

#include <debian-installer/system/dpkg.h>
#include <debian-installer/system/dpkg_internal.h>

#include <debian-installer/exec.h>
#include <debian-installer/system/packages.h>

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int di_system_dpkg_package_configure (di_packages *status, const char *_package, bool force)
{
  di_package *package;
  /* i don't think that is correct, but who cares? */
  const char *argv_config[] = { "configure", NULL };
  const char *argv_postinst[] = { "configure", NULL };
  int ret;

  package = di_packages_get_package (status, _package, 0);
  if (!package)
    return -1;

  switch (package->status)
  {
    case di_package_status_unpacked:
    case di_package_status_half_configured:
      break;
    case di_package_status_installed:
      if (force)
        break;
    default:
      return 1;
  }

  package->status = di_package_status_half_configured;

  ret = internal_di_system_dpkg_package_control_file_exec (package, "config", sizeof (argv_config), argv_config);
  if (ret)
    return -2;
  ret = internal_di_system_dpkg_package_control_file_exec (package, "postinst", sizeof (argv_postinst), argv_postinst);
  if (ret)
    return -3;

  package->status = di_package_status_installed;

  return 0;
}

int internal_di_system_dpkg_package_control_file_exec (di_package *package, const char *name, int argc, const char *const argv[])
{
  char buf[PATH_MAX];
  const char *real_argv[argc + 1];
  int i;
  struct stat statbuf;

  snprintf (buf, sizeof (buf), "%s%s.%s", DI_SYSTEM_DPKG_INFODIR, package->key.string, name);

  if (stat (buf, &statbuf))
    return 0;
  if (!S_ISREG (statbuf.st_mode))
    return 0;

  real_argv[0] = buf;
  for (i = 0; i < argc; i++)
    real_argv[i+1] = argv[i];

  return di_exec (buf, real_argv);
}

int di_system_dpkg_package_control_file_exec (di_packages *status, const char *_package, const char *name, int argc, const char *const argv[])
{
  di_package *package;

  package = di_packages_get_package (status, _package, 0);
  if (!package)
    return -1;

  return internal_di_system_dpkg_package_control_file_exec (package, name, argc, argv);
}

int internal_di_system_dpkg_package_unpack_control (di_package **package, const char *filename, di_packages_allocator *allocator)
{
  const char *argv_rm[] = { "/bin/rm", "-rf", NULL, NULL };
  char buf[PATH_MAX];
  char buf_infodir[PATH_MAX] = { '\0' };
  char buf_tmpdir[PATH_MAX] = DI_SYSTEM_DPKG_TMPCONTROLDIR;
  char *infodir_rest, *tmpdir_rest;
  di_ksize_t infodir_len, infodir_rest_len, tmpdir_len, tmpdir_rest_len;
  DIR *tmpdir;
  struct dirent *tmpdirent;
  struct stat statbuf;

  snprintf (buf_infodir, sizeof (buf_infodir) - 10, "%s%s.", DI_SYSTEM_DPKG_INFODIR, (*package)->key.string);
  infodir_len = strnlen (buf_infodir, sizeof (buf_infodir));

  infodir_rest = buf_infodir + infodir_len;
  infodir_rest_len = sizeof (buf_infodir) - infodir_len;

  tmpdir_len = strnlen (buf_tmpdir, sizeof (buf_tmpdir));

  tmpdir_rest = buf_tmpdir + tmpdir_len;
  tmpdir_rest_len = sizeof (buf_tmpdir) - tmpdir_len;

  if (!stat (buf_tmpdir, &statbuf))
  {
    argv_rm[2] = buf_tmpdir;
    if (di_exec (argv_rm[0], argv_rm))
      return -1;
  }

  if (mkdir (buf_tmpdir, 0700))
    return -1;
  if (chdir (buf_tmpdir))
    return -1;

  snprintf (buf, sizeof (buf), "ar -p %s control.tar.gz|tar -xzf -", filename);

  if (di_exec_shell (buf))
    return -2;

  tmpdir = opendir (buf_tmpdir);
  if (!tmpdir)
    return -2;

  while ((tmpdirent = readdir (tmpdir)))
  {
    if (strchr (tmpdirent->d_name, '.'))
      continue;
    if (strlen (tmpdirent->d_name) > (tmpdir_rest_len < infodir_rest_len ? tmpdir_rest_len : infodir_rest_len))
      continue;
    if (!strcmp (tmpdirent->d_name, "control"))
      if (*package)
        di_package_destroy (*package);
      *package = di_system_package_read_file (buf_tmpdir, allocator);
      continue;

    strcpy (infodir_rest, tmpdirent->d_name);
    strcpy (tmpdir_rest, tmpdirent->d_name);

    if (rename (buf_tmpdir, buf_infodir))
      return -2;
  }

  closedir (tmpdir);

  tmpdir_rest[0] = '\0';
  argv_rm[2] = buf_tmpdir;
  if (di_exec (argv_rm[0], argv_rm))
    return -3;

  return 0;
}

int internal_di_system_dpkg_package_unpack_data (di_package *package, const char *filename)
{
  return 0;
}

int di_system_dpkg_package_unpack (di_packages *status, const char *_package, const char *filename, di_packages_allocator *allocator)
{
  di_package *package;
  int ret;

  package = di_packages_get_package (status, _package, 0);

  if (package && package->status != di_package_status_not_installed)
    return 1;

  ret = internal_di_system_dpkg_package_unpack_control (&package, filename, allocator);
  ret = internal_di_system_dpkg_package_unpack_data (package, filename);

  package->status = di_package_status_unpacked;

  return ret;
}

