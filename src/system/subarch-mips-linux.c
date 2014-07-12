#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <debian-installer/system/subarch.h>

struct hwmap_type {
	const char *sys_regex;
	const char *cpu_regex;
	const char *ret;
};

static struct hwmap_type hwmap[] = {
	{"MIPS Malta",		"MIPS (4K|24K|34K|74K)",	"4kc-malta" },
	{"MIPS Malta",		"MIPS (5K|20K)",		"5kc-malta" },
	{".*",			"ICT Loongson-2 V0.2",		"loongson-2e" },
	{".*",			"ICT Loongson-2 V0.3",		"loongson-2f" },
	{".*",			"ICT Loongson-3",		"loongson-3" },
	{".*",			"Cavium Octeon",		"octeon" },
	{"SGI (Indy|Indigo2)",	"R4",				"r4k-ip22" },
	{"SGI (Indy|Indigo2)",	"R5000",			"r5k-ip22" },
	{"SGI (Indy|Indigo2)",	"R8000",			"r8k-ip26" },
	{"SGI (Indy|Indigo2)",	"R10000",			"r10k-ip28" },
	{"SGI Origin",		"R10000",			"r10k-ip27" },
	{"SGI Origin",		"R12000",			"r12k-ip27" },
	{"SGI (O2|IP32)",	"R5|Nevada|RM7000",		"r5k-ip32" },
	{"SGI (O2|IP32)",	"RM10000",			"r10k-ip32" },
	{"SGI (O2|IP32)",	"RM12000",			"r12k-ip32" },
	{"SiByte BCM91250",	"SiByte SB1",			"sb1-bcm91250a" },
	{"SiByte BCM91.80",	"SiByte SB1A",			"sb1a-bcm91480b" },

	/* add new hwmap here */

	{ ".*",			".*",				"unknown" }
};

#define BUFFER_LENGTH (1024)

const char *di_system_subarch_analyze(void)
{
	FILE *file;
	char buf[BUFFER_LENGTH];
	const char *pos;
	char *system = NULL, *cpu = NULL;
	size_t len, i;

	file = fopen("/proc/cpuinfo", "r");

	if (file) {
		while (fgets(buf, sizeof(buf), file)) {
			if (!(pos = strchr(buf, ':')))
				continue;
			if (!(len = strspn(pos, ": \t")))
				continue;
			if (!strncmp(buf, "system type", strlen("system type")))
				system = strdup(pos + len);
			if (!strncmp(buf, "cpu model", strlen("cpu model")))
				cpu = strdup(pos + len);
		}

		fclose(file);
	}

	if (!system)
		system = strdup("");
	if (!cpu)
		cpu = strdup("");

	for (i = 0; i < sizeof(hwmap) / sizeof(struct hwmap_type) ; i++) {
		regex_t preg;
		int ret;

		/* Check for matching system type */
		if (regcomp(&preg, hwmap[i].sys_regex, REG_NOSUB | REG_EXTENDED))
			continue;
		ret = regexec(&preg, system, 0, NULL, 0);
		regfree(&preg);
		if (ret == REG_NOMATCH)
			continue;

		/* Check for matching cpu type */
		if (regcomp(&preg, hwmap[i].cpu_regex, REG_NOSUB | REG_EXTENDED))
			continue;
		ret = regexec(&preg, cpu, 0, NULL, 0);
		regfree(&preg);
		if (ret == REG_NOMATCH)
			continue;

		/* Match both system and cpu type. Stop. */
		break;
	}

	free(system);
	free(cpu);

	return hwmap[i].ret;
}
