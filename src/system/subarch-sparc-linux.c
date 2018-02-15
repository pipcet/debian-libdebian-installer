#include <debian-installer/system/subarch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH (1024)

const char *di_system_subarch_analyze(void) 
{
  FILE *file;
  char buf[BUFFER_LENGTH];
  const char *pos;
  char *subarch = NULL;
  const char *label = NULL;
  size_t len;

  file = fopen("/proc/cpuinfo", "r");

  if (file) {
    while (fgets(buf, sizeof(buf), file)) {
      if (!(pos = strchr(buf, ':')))
	continue;
      if (!(len = strspn(pos, ": \t")))
	continue;
      if (!strncmp(buf, "type", strlen("type")))
	subarch = strndup(pos + len, strlen(pos + len) - 1);
    }

    fclose(file);
  }

  if (!subarch)
    subarch = strdup("unknown");

  if (access("/sys/firmware/devicetree/base/packages/disk-label/gpt", F_OK) != -1)
    label = "gpt";
  else
    label = "sun";

  snprintf(buf, BUFFER_LENGTH, "%s_%s", subarch, label);

  free(subarch);

  return buf;
}
