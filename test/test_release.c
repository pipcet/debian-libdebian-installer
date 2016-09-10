#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <debian-installer/release.h>

#include "test.h"

const char input_default[] = "\
Origin: Debian\n\
Label: Debian\n\
Suite: unstable\n\
Codename: sid\n\
Version: x.y\n\
Acquire-By-Hash: yes\n\
Description: Debian x.y Unstable - Not Released\n\
SHA256:\n\
 c048039669653f7f126f1d9bea9942101b91cd69344c6db33e8bef9cd4893b80  1441599 contrib/Contents-amd64\n\
 39a5e7c71560909adf8057d40b4469f4b3d55e641b859d8cb669ec7794a3720e   112577 contrib/Contents-amd64.gz\n\
";

const char input_nobyhash[] = "\
SHA256:\n\
 c048039669653f7f126f1d9bea9942101b91cd69344c6db33e8bef9cd4893b80  1441599 contrib/Contents-amd64\n\
 39a5e7c71560909adf8057d40b4469f4b3d55e641b859d8cb669ec7794a3720e   112577 contrib/Contents-amd64.gz\n\
";

di_release *release;

START_TEST(test_get_codename)
{
  ck_assert_ptr_eq(di_release_get_codename(NULL), NULL);
  ck_assert_str_eq(di_release_get_codename(release), "sid");
}
END_TEST

START_TEST(test_get_description)
{
  ck_assert_ptr_eq(di_release_get_description(NULL), NULL);
  ck_assert_str_eq(di_release_get_description(release), "Debian x.y Unstable - Not Released");
}
END_TEST

START_TEST(test_get_label)
{
  ck_assert_ptr_eq(di_release_get_label(NULL), NULL);
  ck_assert_str_eq(di_release_get_label(release), "Debian");
}
END_TEST

START_TEST(test_get_origin)
{
  ck_assert_ptr_eq(di_release_get_origin(NULL), NULL);
  ck_assert_str_eq(di_release_get_origin(release), "Debian");
}
END_TEST

START_TEST(test_get_suite)
{
  ck_assert_ptr_eq(di_release_get_suite(NULL), NULL);
  ck_assert_str_eq(di_release_get_suite(release), "unstable");
}
END_TEST

START_TEST(test_get_version)
{
  ck_assert_ptr_eq(di_release_get_version(NULL), NULL);
  ck_assert_str_eq(di_release_get_version(release), "x.y");
}
END_TEST

START_TEST(test_get_file)
{
  di_release_file fu = { 0 };
  ck_assert_int_eq(di_release_get_file(&fu, release, "contrib/Contents-amd64.unknown"), 0);
  ck_assert_ptr_eq(fu.name, NULL);
  ck_assert_ptr_eq(fu.name_byhash, NULL);
  ck_assert_int_eq(fu.checksum.type, 0);
  ck_assert_ptr_eq(fu.checksum.value, NULL);

  di_release_file fk;
  ck_assert_int_eq(di_release_get_file(&fk, release, "contrib/Contents-amd64.gz"), 1);
  ck_assert_str_eq(fk.name, "contrib/Contents-amd64.gz");
  ck_assert_str_eq(fk.name_byhash, "contrib/by-hash/SHA256/39a5e7c71560909adf8057d40b4469f4b3d55e641b859d8cb669ec7794a3720e");
  ck_assert_int_eq(fk.checksum.type, DI_RELEASE_FILE_CHECKSUM_SHA256);
  ck_assert_str_eq(fk.checksum.value, "39a5e7c71560909adf8057d40b4469f4b3d55e641b859d8cb669ec7794a3720e");
}
END_TEST

START_TEST(test_get_file_nobyhash)
{
  di_release_file fk;
  ck_assert_int_eq(di_release_get_file(&fk, release, "contrib/Contents-amd64.gz"), 1);
  ck_assert_str_eq(fk.name, "contrib/Contents-amd64.gz");
  ck_assert_ptr_eq(fk.name_byhash, NULL);
  ck_assert_int_eq(fk.checksum.type, DI_RELEASE_FILE_CHECKSUM_SHA256);
  ck_assert_str_eq(fk.checksum.value, "39a5e7c71560909adf8057d40b4469f4b3d55e641b859d8cb669ec7794a3720e");
}
END_TEST

static void setup_default() {
  release = di_release_read(input_default, strlen(input_default));
  ck_assert_ptr_ne(release, NULL);
}

static void setup_nobyhash() {
  release = di_release_read(input_nobyhash, strlen(input_nobyhash));
  ck_assert_ptr_ne(release, NULL);
}

static void teardown() {
  di_release_free(release);
}

Suite* make_test_release_suite() {
  Suite *s = suite_create("test release");
  TCase *tc;

  tc = tcase_create("default");
  tcase_add_checked_fixture(tc, setup_default, teardown);
  tcase_add_test(tc, test_get_codename);
  tcase_add_test(tc, test_get_description);
  tcase_add_test(tc, test_get_label);
  tcase_add_test(tc, test_get_origin);
  tcase_add_test(tc, test_get_suite);
  tcase_add_test(tc, test_get_version);
  tcase_add_test(tc, test_get_file);
  suite_add_tcase(s, tc);

  tc = tcase_create("nobyhash");
  tcase_add_checked_fixture(tc, setup_nobyhash, teardown);
  tcase_add_test(tc, test_get_file_nobyhash);
  suite_add_tcase(s, tc);

  return s;
}
