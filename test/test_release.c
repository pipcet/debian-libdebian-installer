#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <debian-installer/release.h>

#include "test.h"

const char input[] = "\
Origin: Debian\n\
Label: Debian\n\
Suite: unstable\n\
Codename: sid\n\
Version: x.y\n\
Date: Sun, 04 Sep 2016 09:30:08 UTC\n\
Valid-Until: Sun, 11 Sep 2016 09:30:08 UTC\n\
Acquire-By-Hash: yes\n\
Architectures: amd64 arm64 armel armhf hurd-i386 i386 kfreebsd-amd64 kfreebsd-i386 mips mips64el mipsel powerpc ppc64el s390x\n\
Components: main contrib non-free\n\
Description: Debian x.y Unstable - Not Released\n\
SHA256:\n\
 c048039669653f7f126f1d9bea9942101b91cd69344c6db33e8bef9cd4893b80  1441599 contrib/Contents-amd64\n\
 64af05653654e690cbfd73021463b0fd4c25329b1e184013fcb19a94a91c903b    27796 contrib/Contents-amd64.diff/Index\n\
 39a5e7c71560909adf8057d40b4469f4b3d55e641b859d8cb669ec7794a3720e   112577 contrib/Contents-amd64.gz\n\
";
size_t input_len = strlen(input);

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
  di_release_file fu = di_release_get_file(release, "contrib/Contents-amd64.unknown");
  ck_assert_ptr_eq(fu.name, NULL);
  ck_assert_ptr_eq(fu.name_byhash, NULL);
  ck_assert_int_eq(fu.checksum.type, 0);
  ck_assert_ptr_eq(fu.checksum.value, NULL);

  di_release_file fk = di_release_get_file(release, "contrib/Contents-amd64.gz");
  ck_assert_str_eq(fk.name, "contrib/Contents-amd64.gz");
  ck_assert_str_eq(fk.name_byhash, "contrib/by-hash/SHA256/39a5e7c71560909adf8057d40b4469f4b3d55e641b859d8cb669ec7794a3720e");
  ck_assert_int_eq(fk.checksum.type, DI_RELEASE_FILE_CHECKSUM_SHA256);
  ck_assert_str_eq(fk.checksum.value, "39a5e7c71560909adf8057d40b4469f4b3d55e641b859d8cb669ec7794a3720e");
}
END_TEST

static void setup() {
  release = di_release_read(input, input_len);
  ck_assert_ptr_ne(release, NULL);
}

static void teardown() {
  di_release_free(release);
}

Suite* make_test_release_suite() {
  Suite *s;
  TCase *tc_core;

  s = suite_create("test release");
  tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);
  tcase_add_test(tc_core, test_get_codename);
  tcase_add_test(tc_core, test_get_description);
  tcase_add_test(tc_core, test_get_label);
  tcase_add_test(tc_core, test_get_origin);
  tcase_add_test(tc_core, test_get_suite);
  tcase_add_test(tc_core, test_get_version);
  tcase_add_test(tc_core, test_get_file);
  suite_add_tcase(s, tc_core);

  return s;
}
