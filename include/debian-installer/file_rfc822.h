/*
 * file_rfc822.h - Reader and writer for Debian RFC822 files
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

#ifndef DEBIAN_INSTALLER__FILE_RFC822_H
#define DEBIAN_INSTALLER__FILE_RFC822_H

#include <debian-installer/parser.h>

#include <stdbool.h>
#include <stdio.h>

/**
 * @addtogroup di_file_rfc822
 * @{
 */

/**
 * Read a Debian RFC822 formated file with exactly one entry
 *
 * @param stream file opened for readiningg
 * @param info parser info
 * @param user_data user_data for parser functions
 *
 * @return -1 on error, read entries otherwise
 */
int di_file_rfc822_read_one(FILE *stream, di_parser_info *info, void *user_data);

/**
 * Read a Debian RFC822 formated file
 *
 * @param stream file opened for readiningg
 * @param info parser info
 * @param entry_new function which is called before each entry
 * @param entry_finish function which is called after each entry
 * @param user_data user_data for parser functions
 *
 * @return -1 on error, read entries otherwise
 */
int di_file_rfc822_read_many(FILE *stream, di_parser_info *info, di_parser_read_entry_new entry_new, di_parser_read_entry_finish entry_finish, void *user_data);

/** @} */
#endif
