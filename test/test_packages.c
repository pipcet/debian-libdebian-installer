#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <debian-installer/packages.h>
#include "../src/package_internal.h"

#include "test.h"

static const char input_default[] = "\
Package: test1\n\
Priority: important\n\
Section: libs\n\
Architecture: amd64\n\
Version: 1.1-1\n\
Description: test\n\
Installer-Menu-Item: 20\n\
Kernel-Version: 1.1\n\
Subarchitecture: opteron\n\
\n\
Package: test2\n\
Priority: optional\n\
Section: libs\n\
Architecture: all\n\
Version: 1.1-2\n\
Depends: test\n\
Size: 100\n\
Description: test\n\
Installer-Menu-Item: 20\n\
Kernel-Version: 1.1\n\
Subarchitecture: opteron\n\
";

static const char input_add[] = "\
Package: add\n\
Version: 1.1-2\n\
";

di_packages *packages;

START_TEST(test_add_package)
{
  FILE *f = fmemopen((void *)input_add, strlen(input_add), "r");
  di_package *p1 = di_package_read(f);
  ck_assert_ptr_ne(p1, NULL);
  fclose(f);

  di_packages_add_package(packages, p1);

  const di_package *p2 = di_packages_get_package(packages, "add");
  ck_assert_ptr_eq(p1, p2);
}
END_TEST

START_TEST(test_get_package)
{
  ck_assert_ptr_eq(di_packages_get_package(NULL, NULL), NULL);
  ck_assert_ptr_eq(di_packages_get_package(packages, NULL), NULL);
  ck_assert_ptr_eq(di_packages_get_package(NULL, "test"), NULL);
  ck_assert_ptr_eq(di_packages_get_package(packages, "test"), NULL);

  const di_package *p;
  p = di_packages_get_package(packages, "test1");
  ck_assert_str_eq(p->package, "test1");

  p = di_packages_get_package(packages, "test2");
  ck_assert_str_eq(p->package, "test2");
}
END_TEST

static void setup_default() {
  FILE *f = fmemopen((void *)input_default, strlen(input_default), "r");

  packages = di_packages_new();
  ck_assert_ptr_ne(packages, NULL);
  ck_assert_int_eq(di_packages_read(packages, f), 2);

  fclose(f);
}

static void teardown() {
  di_packages_free(packages);
}

Suite* make_test_packages_suite() {
  Suite *s = suite_create("test packages");
  TCase *tc;

  tc = tcase_create("default");
  tcase_add_checked_fixture(tc, setup_default, teardown);
  tcase_add_test(tc, test_add_package);
  tcase_add_test(tc, test_get_package);
  suite_add_tcase(s, tc);

  return s;
}
