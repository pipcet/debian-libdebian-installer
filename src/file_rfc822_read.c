/*
 * file_rfc822_read.c - Reader Debian RFC822 files
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

#include <debian-installer/file_rfc822.h>

static int read_field(char *data, ssize_t datalen, di_parser_info *info, void **act, void *user_data)
{
  // Remove trailing newline
  data[datalen - 1] = 0;
  datalen--;

  // Search end of field name
  char *field_end = memchr(data, ':', datalen);
  if (!field_end)
    abort();

  // Search begin of value, ignore any whitespaces
  char *value_begin = field_end + 1;
  while (*value_begin == ' ' || *value_begin == '\t')
    value_begin++;

  di_rstring field_string = { data, field_end - data };
  di_rstring value_string = { value_begin, datalen - (value_begin - data) };

  const di_parser_fieldinfo *fip = di_hash_table_lookup(info->table, &field_string);

  if (fip)
  {
    fip->read(act, fip, NULL, &value_string, user_data);
    return 0;
  }

  if (info->wildcard)
  {
    di_rstring empty_string = { "", 0 };

    fip = di_hash_table_lookup (info->table, &empty_string);

    if (fip)
    {
      fip->read(act, fip, &field_string, &value_string, user_data);
      return 0;
    }
  }

  return -1;
}

static bool read_continuation(FILE *stream)
{
  // Peak into next line
  char next = getc(stream);

  // Check for continuation
  if (next == ' ' || next == '\t')
    return true;
  else
  {
    // Stuff the first character back
    ungetc(next, stream);
    return false;
  }
}

static int read_one(FILE *stream, di_parser_info *info, void *act, void *user_data)
{
  size_t linesize = 0;
  char *line = NULL;
  int ret = 0;

  for (;; ret++)
  {
    // We are at a new field, read the first line
    ssize_t linelen = getline(&line, &linesize, stream);

    // Check for EOF or empty line
    if (feof(stream) || linelen == 0 || (linelen == 1 && line[0] == '\n'))
      break;

    // Expect newline
    if (line[linelen - 1] != '\n')
      return -1;

    // Check for continuation
    if (!read_continuation(stream))
    {
      read_field(line, linelen, info, &act, user_data);
    }
    else
    {
      // Allocate temporary place for multiline field
      size_t datasize = linelen * 4;
      char *data = malloc(datasize);

      strncpy(data, line, datasize);
      ssize_t datalen = linelen;

      do
      {
        // We are at the second line of a multiline field
        ssize_t linelen = getline(&line, &linesize, stream);

        // Expect newline
        if (line[linelen - 1] != '\n')
          return -1;

        if (datalen + linelen >= datasize)
        {
          datasize *= 4;
          data = realloc(data, datasize);
        }

        // Append next line
        strncat(data, line, datasize);
        datalen += linelen;

      }
      // Check for continuation
      while (read_continuation(stream));

      read_field(data, datalen, info, &act, user_data);

      free(data);
    }
  }

  free(line);

  return ret;
}

int di_file_rfc822_read_one(FILE *stream, di_parser_info *info, void *user_data)
{
  if (read_one(stream, info, user_data, user_data) > 0 && getc(stream) == EOF)
    return 1;
  return -1;
}

int di_file_rfc822_read_many(FILE *stream, di_parser_info *info, di_parser_read_entry_new entry_new, di_parser_read_entry_finish entry_finish, void *user_data)
{
  int ret = 0;

  while (true)
  {
    void *act = entry_new(user_data);
    if (read_one(stream, info, act, user_data) > 0)
    {
      ret++;
      entry_finish(act, user_data);
    }

    char n = getc(stream);
    if (n != EOF)
      ungetc(n, stream);
    else
      break;
  }
  return ret;
}
