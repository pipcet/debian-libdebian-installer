#include <debian-installer/file_rfc822.h>

#include "test.h"

const char input_empty[] = "\
\n\
";

const char input_one_default[] = "\
Real: test\n\
Real:  test\n\
Real:   test\n\
Multiline: test\n\
 multiline1\n\
\tmultiline2\n\
 multiline3\n\
Wildcard: wildcard\n\
\n\
";

const char input_one_toolong[] = "\
Test: test\n\
\n\
\n\
";

const char input_many[] = "\
Real: test\n\
\n\
Real: test\n\
Multiline: test\n\
 multiline1\n\
\tmultiline2\n\
 multiline3\n\
\n\
";

static di_file_fields_function_read
  real_read, multiline_read, wildcard_read;

const di_file_fieldinfo fieldinfo[] =
{
  DI_FILE_FIELDINFO
  (
    "Real",
    real_read,
    NULL,
    1
  ),
  DI_FILE_FIELDINFO
  (
    "Multiline",
    multiline_read,
    NULL,
    2
  ),
  DI_FILE_FIELDINFO
  (
    "",
    wildcard_read,
    NULL,
    3
  ),
};

struct read_info {
  int found_real, found_multiline, found_wildcard;
  int called_new, called_finish;
};

static void real_read(
  void **data,
  const di_file_fieldinfo *fip,
  di_rstring *field,
  di_rstring *value,
  void *user_data)
{
  struct read_info *i = user_data;
  i->found_real++;
  ck_assert_ptr_eq(fip, &fieldinfo[0]);
  ck_assert_str_eq(value->string, "test");
  ck_assert_int_eq(value->size, strlen("test"));
}

static void multiline_read(
  void **data,
  const di_file_fieldinfo *fip,
  di_rstring *field,
  di_rstring *value,
  void *user_data)
{
  struct read_info *i = user_data;
  i->found_multiline++;
  ck_assert_ptr_eq(fip, &fieldinfo[1]);
  ck_assert_str_eq(value->string, "test\nmultiline1\nmultiline2\nmultiline3");
  ck_assert_int_eq(value->size, strlen("test\nmultiline1\nmultiline2\nmultiline3"));
}

static void wildcard_read(
  void **data,
  const di_file_fieldinfo *fip,
  di_rstring *field,
  di_rstring *value,
  void *user_data)
{
  struct read_info *i = user_data;
  i->found_wildcard++;
  ck_assert_ptr_eq(fip, &fieldinfo[2]);
  ck_assert_str_eq(value->string, "wildcard");
  ck_assert_int_eq(value->size, strlen("wildcard"));
}

di_file_read_entry_new many_new;
di_file_read_entry_finish many_finish;

void *many_new(void *user_data)
{
  struct read_info *i = user_data;
  i->called_new++;
  return user_data;
}

int many_finish(void *data, void *user_data)
{
  struct read_info *i = user_data;
  i->called_finish++;
  ck_assert_ptr_eq(data, user_data);
  return 0;
}

START_TEST(test_empty)
{
  di_file_info *info = di_file_info_alloc();
  di_file_info_add(info, fieldinfo, sizeof(fieldinfo)/sizeof(fieldinfo[0]));

  FILE *f = fmemopen((void *)input_empty, strlen(input_empty), "r");

  ck_assert_int_eq(di_file_rfc822_read_one(f, info, NULL), -1);

  fclose(f);
  di_file_info_free(info);
}
END_TEST

START_TEST(test_one_default)
{
  di_file_info *info = di_file_info_alloc();
  di_file_info_add(info, fieldinfo, sizeof(fieldinfo)/sizeof(fieldinfo[0]));
  struct read_info read_info = { 0, };

  FILE *f = fmemopen((void *)input_one_default, strlen(input_one_default), "r");

  ck_assert_int_eq(di_file_rfc822_read_one(f, info, &read_info), 1);
  ck_assert_int_eq(read_info.found_real, 3);
  ck_assert_int_eq(read_info.found_multiline, 1);
  ck_assert_int_eq(read_info.found_wildcard, 1);

  fclose(f);
  di_file_info_free(info);
}
END_TEST

START_TEST(test_many)
{
  di_file_info *info = di_file_info_alloc();
  di_file_info_add(info, fieldinfo, sizeof(fieldinfo)/sizeof(fieldinfo[0]));
  struct read_info read_info = { 0, };

  FILE *f = fmemopen((void *)input_many, strlen(input_many), "r");

  ck_assert_int_eq(di_file_rfc822_read_many(f, info, many_new, many_finish, &read_info), 2);
  ck_assert_int_eq(read_info.called_new, 2);
  ck_assert_int_eq(read_info.called_finish, 2);
  ck_assert_int_eq(read_info.found_real, 2);
  ck_assert_int_eq(read_info.found_multiline, 1);
  ck_assert_int_eq(read_info.found_wildcard, 0);

  fclose(f);
  di_file_info_free(info);
}
END_TEST

Suite* make_test_file_rfc822_suite() {
  Suite *s = suite_create("test file_rfc822");
  TCase *tc;

  tc = tcase_create("empty");
  tcase_add_test(tc, test_empty);
  suite_add_tcase(s, tc);

  tc = tcase_create("one");
  tcase_add_test(tc, test_one_default);
  suite_add_tcase(s, tc);

  tc = tcase_create("many");
  tcase_add_test(tc, test_many);
  suite_add_tcase(s, tc);

  return s;
}
