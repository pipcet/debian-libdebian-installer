/*
 * dpkg.h
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
 * $Id: dpkg.h,v 1.3 2003/09/24 11:49:52 waldi Exp $
 */

#ifndef DEBIAN_INSTALLER__SYSTEM__DPKG_H
#define DEBIAN_INSTALLER__SYSTEM__DPKG_H

#include <debian-installer/packages.h>

#include <stdbool.h>

#define DI_SYSTEM_DPKG_ADMINDIR "/var/lib/dpkg/"
#define DI_SYSTEM_DPKG_INFODIR DI_SYSTEM_DPKG_ADMINDIR "info/"
#define DI_SYSTEM_DPKG_STATUSFILE DI_SYSTEM_DPKG_ADMINDIR "status"
#define DI_SYSTEM_DPKG_TMPCONTROLDIR DI_SYSTEM_DPKG_ADMINDIR "tmp.ci/"

int di_system_dpkg_package_configure (di_packages *status, const char *package, bool force);
int di_system_dpkg_package_control_file_exec (di_packages *status, const char *package, const char *name, int argc, const char *const argv[]);
int di_system_dpkg_package_unpack (di_packages *status, const char *package, const char *filename, di_packages_allocator *allocator);

#endif
