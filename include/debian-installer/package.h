/*
 * package.h
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

#ifndef DEBIAN_INSTALLER__PACKAGE_H
#define DEBIAN_INSTALLER__PACKAGE_H

/**
 * @addtogroup di_package
 * @{
 */

/**
 * Priority field
 */
enum di_package_priority
{
  di_package_priority_extra = 1,
  di_package_priority_optional,
  di_package_priority_standard,
  di_package_priority_important,
  di_package_priority_required,
};

/**
 * Status field, third part
 */
enum di_package_status
{
  di_package_status_undefined = 0,
  di_package_status_not_installed,
  di_package_status_unpacked,
  di_package_status_installed,
  di_package_status_half_configured,
  di_package_status_config_files,
};

/**
 * Status field, first part
 */
enum di_package_status_want
{
  di_package_status_want_unknown = 0,
  di_package_status_want_install,
  di_package_status_want_hold,
  di_package_status_want_deinstall,
  di_package_status_want_purge,
};

/**
 * Type of dependency
 */
enum di_package_dependency_type
{
  di_package_dependency_type_replaces = 1,              /**< Replaces field */
  di_package_dependency_type_provides,                  /**< Provides field */
  di_package_dependency_type_depends,                   /**< Depends field */
  di_package_dependency_type_pre_depends,               /**< Pre-Depends field */
  di_package_dependency_type_recommends,                /**< Recommends field */
  di_package_dependency_type_suggests,                  /**< Suggests field */
  di_package_dependency_type_conflicts,                 /**< Conflicts field */
  di_package_dependency_type_enhances,                  /**< Enhances field */
  di_package_dependency_type_reverse_provides = 0x100,  /**< @internal */
  di_package_dependency_type_reverse_enhances,          /**< @internal */
};

/** @} */
#endif
