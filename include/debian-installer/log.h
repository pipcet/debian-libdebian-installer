/*
 * log.h
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
 * $Id: log.h,v 1.1 2003/08/29 12:37:33 waldi Exp $
 */

#ifndef DEBIAN_INSTALLER__LOG_H
#define DEBIAN_INSTALLER__LOG_H

#include <stdarg.h>

/**
 * @defgroup di_log Logging
 * @{
 */

typedef enum
{
  DI_LOG_FLAG_FATAL             = 1 << 1,

  DI_LOG_LEVEL_ERROR            = 1 << 2,       /* always fatal */
  DI_LOG_LEVEL_CRITICAL         = 1 << 3,
  DI_LOG_LEVEL_WARNING          = 1 << 4,
  DI_LOG_LEVEL_MESSAGE          = 1 << 5,
  DI_LOG_LEVEL_INFO             = 1 << 6,
  DI_LOG_LEVEL_DEBUG            = 1 << 7,
  DI_LOG_LEVEL_OUTPUT           = 1 << 8,

  DI_LOG_LEVEL_MASK             = ~DI_LOG_FLAG_FATAL,
  DI_LOG_FATAL_MASK             = DI_LOG_LEVEL_ERROR,
}
di_log_level_flags;

typedef void di_log_handler (di_log_level_flags log_level, const char *message, void *user_data);

/**
 * logs an error
 */
#define di_error(format...) di_log (DI_LOG_LEVEL_ERROR, format)
/**
 * logs a warning
 */
#define di_warning(format...) di_log (DI_LOG_LEVEL_WARNING, format)

void di_log (di_log_level_flags log_level, const char *format, ...) __attribute__ ((format(printf,2,3)));
void di_logv (di_log_level_flags log_level, const char *format, va_list args);

unsigned int di_log_set_handler (di_log_level_flags log_levels, di_log_handler *log_func, void *user_data);

di_log_handler
  di_log_handler_syslog;

/** @} */
#endif