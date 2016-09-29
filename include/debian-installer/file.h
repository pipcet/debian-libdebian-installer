/*
 * file.h - Generic file support
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

#ifndef DEBIAN_INSTALLER__FILE_H
#define DEBIAN_INSTALLER__FILE_H

#include <debian-installer/hash.h>
#include <debian-installer/slist.h>
#include <debian-installer/string.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void di_file_info;
typedef struct di_file_fieldinfo di_file_fieldinfo;

/**
 * @addtogroup di_file
 * @{
 */

/**
 * Read a single field
 *
 * @param data the actual data
 * @param fip info of the actual field
 * @param value the actual value
 * @param value_size size of the actual value
 * @param user_data data supplied to the parser
 */
typedef void di_file_fields_function_read(void **data, const di_file_fieldinfo *fip, di_rstring *field, di_rstring *value, void *user_data);

/**
 * Write a single field - callback
 *
 * @param field the field
 * @param value the value of the field
 * @param data the callback_data
 */
typedef void di_file_fields_function_write_callback(const char *field, const di_rstring *value, void *data);

/**
 * Write a single field
 *
 * @param data the actual data
 * @param fip info of the actual field
 * @param output static buffer for output
 * @param user_data data supplied to the parser
 *
 * @return written bytes
 */
typedef void di_file_fields_function_write(void **data, const di_file_fieldinfo *fip, di_file_fields_function_write_callback callback, void *callback_data, void *user_data);

/**
 * @param user_data data supplied to di_parse
 * @return new data
 */
typedef void *di_file_read_entry_new(void *user_data);

/**
 * @param data the actual data
 * @param user_data data supplied to di_parse
 */
typedef int di_file_read_entry_finish(void *data, void *user_data);

/**
 * @param data the actual data
 * @param user_data data supplied to di_parse
 */
typedef void *di_file_write_entry_next(void **state_data, void *user_data);

/**
 * @brief Info about a parser field
 */
struct di_file_fieldinfo
{
  const char *key;                                      /**< field name */
  di_file_fields_function_read *read;                   /**< function for reading a field */
  di_file_fields_function_write *write;                 /**< function for writing a field */
  unsigned int integer;                                 /**< Simple value, usage is defined by the read and write functions.
                                                         *   Most used with an offset of the field in the structure. */
};

/**
 * generates a di_file_fieldinfo
 */
#define DI_FILE_FIELDINFO(name, read, write, integer) \
  { name, read, write, integer }

di_file_fields_function_read
  /**
   * Read function for a boolean (true == "Yes")
   */
  di_file_read_boolean,
  /**
   * Read function for an int
   */
  di_file_read_int,
  /**
   * Read function for a di_rstring
   */
  di_file_read_rstring,
  /**
   * Read function for a string
   */
  di_file_read_string;

di_file_fields_function_write
  /**
   * Write function for a boolean ("Yes" == true)
   */
  di_file_write_boolean,
  /**
   * Write function for an int
   */
  di_file_write_int,
  /**
   * Write function for a di_string
   */
  di_file_write_rstring,
  /**
   * Write function for a string
   */
  di_file_write_string;

di_file_info *di_file_info_alloc(void);
void di_file_info_free(di_file_info *info);

void di_file_info_add(di_file_info *info, const di_file_fieldinfo *fieldinfo[]);

/** @} */
#endif
