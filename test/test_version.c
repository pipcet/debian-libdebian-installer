#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <debian-installer/version.h>

#include "test.h"

START_TEST(test_parse_simple)
{
  di_version *v = di_version_parse("1");
  ck_assert_ptr_ne(v, NULL);
  ck_assert_int_eq(v->epoch, 0);
  ck_assert_str_eq(v->upstream, "1");
  ck_assert_ptr_eq(v->revision, NULL);
  di_version_free(v);
}
END_TEST

START_TEST(test_parse_complex)
{
  di_version *v = di_version_parse("1:2.3-4.5");
  ck_assert_ptr_ne(v, NULL);
  ck_assert_int_eq(v->epoch, 1);
  ck_assert_str_eq(v->upstream, "2.3");
  ck_assert_str_eq(v->revision, "4.5");
  di_version_free(v);
}
END_TEST

START_TEST(test_cmp)
{
  di_version *v1 = di_version_parse("1:2.3-4.5");
  di_version *v2 = di_version_parse("1:2.3-4.6");
  di_version *v3 = di_version_parse("2:2.3-4.4");
  ck_assert_ptr_ne(v1, NULL);
  ck_assert_ptr_ne(v2, NULL);
  ck_assert_ptr_ne(v3, NULL);
  ck_assert_int_eq(di_version_cmp(v1, v1), 0);
  ck_assert_int_eq(di_version_cmp(v2, v2), 0);
  ck_assert_int_eq(di_version_cmp(v3, v3), 0);
  ck_assert(di_version_cmp(v1, v2) < 0);
  ck_assert(di_version_cmp(v1, v3) < 0);
  ck_assert(di_version_cmp(v2, v3) < 0);
  ck_assert(di_version_cmp(v2, v1) > 0);
  ck_assert(di_version_cmp(v3, v1) > 0);
  ck_assert(di_version_cmp(v3, v2) > 0);
  di_version_free(v1);
  di_version_free(v2);
  di_version_free(v3);
}
END_TEST

Suite* make_test_version_suite() {
  Suite *s = suite_create("test version");
  TCase *tc;

  tc = tcase_create("default");
  tcase_add_test(tc, test_parse_simple);
  tcase_add_test(tc, test_parse_complex);
  tcase_add_test(tc, test_cmp);
  suite_add_tcase(s, tc);

  return s;
}
