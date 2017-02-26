#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <debian-installer/package.h>

#include "test.h"

static const char input_default[] = "\
Package: test\n\
Priority: important\n\
Section: libs\n\
Architecture: amd64\n\
Version: 1.1-1\n\
Replaces: replace\n\
Depends: libc6 (>= 2.17)\n\
Pre-Depends: multiarch-support\n\
Filename: test.deb\n\
Size: 100\n\
Description: test\n\
SHA256: c048039669653f7f126f1d9bea9942101b91cd69344c6db33e8bef9cd4893b80\n\
Installer-Menu-Item: 20\n\
Kernel-Version: 1.1\n\
Subarchitecture: opteron\n\
";

di_package *package;

START_TEST(test_get_package)
{
  ck_assert_ptr_eq(di_package_get_package(NULL), NULL);
  ck_assert_str_eq(di_package_get_package(package), "test");
}
END_TEST

START_TEST(test_get_priority)
{
  ck_assert_int_eq(di_package_get_priority(NULL), 0);
  ck_assert_int_eq(di_package_get_priority(package), di_package_priority_important);
}
END_TEST

START_TEST(test_get_section)
{
  ck_assert_ptr_eq(di_package_get_section(NULL), NULL);
  ck_assert_str_eq(di_package_get_section(package), "libs");
}
END_TEST

START_TEST(test_get_architecture)
{
  ck_assert_ptr_eq(di_package_get_architecture(NULL), NULL);
  ck_assert_str_eq(di_package_get_architecture(package), "amd64");
}
END_TEST

START_TEST(test_get_version)
{
  ck_assert_ptr_eq(di_package_get_version(NULL), NULL);
  ck_assert_str_eq(di_package_get_version(package)->upstream, "1.1");
}
END_TEST

START_TEST(test_get_relation)
{
  ck_assert_ptr_eq(di_package_get_relation(NULL, 0), NULL);
}
END_TEST

START_TEST(test_get_relation_type_replaces)
{
  ck_assert_str_eq(di_package_get_relation(package, di_package_relation_type_replaces), "replace");
}
END_TEST

START_TEST(test_get_relation_type_depends)
{
  ck_assert_str_eq(di_package_get_relation(package, di_package_relation_type_depends), "libc6 (>= 2.17)");
}
END_TEST

START_TEST(test_get_relation_type_predepends)
{
  ck_assert_str_eq(di_package_get_relation(package, di_package_relation_type_predepends), "multiarch-support");
}
END_TEST

START_TEST(test_get_filename)
{
  ck_assert_ptr_eq(di_package_get_filename(NULL), NULL);
  ck_assert_str_eq(di_package_get_filename(package), "test.deb");
}
END_TEST

START_TEST(test_get_size)
{
  ck_assert_int_eq(di_package_get_size(NULL), 0);
  ck_assert_int_eq(di_package_get_size(package), 100);
}
END_TEST

START_TEST(test_get_description)
{
  ck_assert_ptr_eq(di_package_get_description(NULL), NULL);
  ck_assert_str_eq(di_package_get_description(package), "test");
}
END_TEST

START_TEST(test_get_digest)
{
  ck_assert_ptr_eq(di_package_get_digest(NULL), NULL);
  ck_assert_int_eq(di_package_get_digest(package)->type, DI_DIGEST_SHA256);
  ck_assert_str_eq(di_package_get_digest(package)->value, "c048039669653f7f126f1d9bea9942101b91cd69344c6db33e8bef9cd4893b80");
}
END_TEST

START_TEST(test_get_di_installermenuitem)
{
  ck_assert_int_eq(di_package_get_di_installermenuitem(NULL), 0);
  ck_assert_int_eq(di_package_get_di_installermenuitem(package), 20);
}
END_TEST

START_TEST(test_get_di_kernelversion)
{
  ck_assert_ptr_eq(di_package_get_di_kernelversion(NULL), NULL);
  ck_assert_str_eq(di_package_get_di_kernelversion(package), "1.1");
}
END_TEST

START_TEST(test_get_di_subarchitecture)
{
  ck_assert_ptr_eq(di_package_get_di_subarchitecture(NULL), NULL);
  ck_assert_str_eq(di_package_get_di_subarchitecture(package), "opteron");
}
END_TEST

static void setup_default() {
  FILE *f = fmemopen((void *)input_default, strlen(input_default), "r");

  package = di_package_read(f);
  ck_assert_ptr_ne(package, NULL);

  fclose(f);
}

static void teardown() {
  di_package_free(package);
}

Suite* make_test_package_suite() {
  Suite *s = suite_create("test package");
  TCase *tc;

  tc = tcase_create("default");
  tcase_add_checked_fixture(tc, setup_default, teardown);
  tcase_add_test(tc, test_get_package);
  tcase_add_test(tc, test_get_priority);
  tcase_add_test(tc, test_get_section);
  tcase_add_test(tc, test_get_architecture);
  tcase_add_test(tc, test_get_version);
  tcase_add_test(tc, test_get_relation);
  tcase_add_test(tc, test_get_relation_type_replaces);
  tcase_add_test(tc, test_get_relation_type_depends);
  tcase_add_test(tc, test_get_relation_type_predepends);
  tcase_add_test(tc, test_get_filename);
  tcase_add_test(tc, test_get_size);
  tcase_add_test(tc, test_get_description);
  tcase_add_test(tc, test_get_digest);
  tcase_add_test(tc, test_get_di_installermenuitem);
  tcase_add_test(tc, test_get_di_kernelversion);
  tcase_add_test(tc, test_get_di_subarchitecture);
  suite_add_tcase(s, tc);

  return s;
}
