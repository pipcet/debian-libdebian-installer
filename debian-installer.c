/* 
   debian-installer.c - common utilities for debian-installer
   Author - David Kimdon

   Copyright (C) 2000-2002  David Kimdon <dwhedon@debian.org>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include "debian-installer.h"



#define PREBASECONFIG_D "/usr/lib/prebaseconfig.d"

/*
   di_prebaseconfig_append()

   Append to a script in /usr/lib/prebaseconfig.d/.  All the scripts in this
   directory will be run immediately before the system is rebooted.  udebs drop
   commands in here that they know need to be run at this time.  
   
   udeb - Identifier, most likely the udeb making the call
   
   format, . . . - printf formatted text that will be appended to
   /usr/lib/prebaseconfig.d/<udeb>
*/
int
di_prebaseconfig_append(const char *udeb, const char *fmt, ...)
{
  char *path = NULL;
  FILE *fp = NULL;
  int rv = -1;
  va_list ap;
  time_t t;
  
  if (asprintf (&path, PREBASECONFIG_D "/%s", udeb) == -1) 
   {
    perror ("di_prebaseconfig_append: asprintf");
    goto finished;
   }
  
  if ( (fp = fopen (path, "a")) == NULL) 
   {
    perror ("di_prebaseconfig_append: fopen");
    goto finished;
   }
  
  time(&t);
  fprintf(fp, "\n# start entry %s\n", ctime(&t));
  
  va_start(ap, fmt);
  vfprintf(fp, fmt, ap);
  va_end(ap);
  
  fprintf(fp, "\n# end entry\n");
  
  rv = 0;
  
finished:
  free(path);
  if (fp)
    fclose(fp);
  return rv;
}



/* di_execlog():
   execute the command, log the result
   */

int
di_execlog (const char *incmd)
{
  FILE *output;
  char cmd[strlen (incmd) + 6];
  char line[MAXLINE];
  strcpy (cmd, incmd);

  openlog ("installer", LOG_PID | LOG_PERROR, LOG_USER);
  syslog (LOG_DEBUG, "running cmd '%s'", cmd);

  /* FIXME: this can cause the shell command if there's redirection
     already in the passed string */
  strcat (cmd, " 2>&1");
  output = popen (cmd, "r");
  while (fgets (line, MAXLINE, output) != NULL)
    {
      syslog (LOG_DEBUG, line);
    }
  closelog ();
  /* FIXME we aren't getting the return value from the actual command
     executed, not sure how to do that cleanly */
  return (pclose (output));
}


void
di_log(const char *msg){
    openlog ("installer", LOG_PID | LOG_PERROR, LOG_USER);
    syslog (LOG_DEBUG, "%s", msg);
    closelog ();
}



int
di_check_dir (const char *dirname)
{
  struct stat check;
  if (stat (dirname, &check) == -1)
    return -1;
  else
    return S_ISDIR (check.st_mode);
}


int
di_snprintfcat (char *str, size_t size, const char *format, ...)
{
  va_list ap;
  int retval;
  size_t len = strlen (str);

  va_start (ap, format);
  retval = vsnprintf (str + len, size - len, format, ap);
  va_end (ap);

  return retval;
}

char *
di_stristr(const char *haystack, const char *needle)
{
    /* I suppose using strncasecmp is suboptimal, but we are already
     * using GNU extensions in other places. */
    const char *ret;
    size_t n_len;

    n_len = strlen(needle);
    for (ret = haystack; *ret != '\0'; ret++)
    {
        if (strncasecmp(ret, needle, n_len) == 0)
            return (char *)ret;
    }
    return NULL;
}

struct package_t *
di_pkg_alloc(const char *name)
{
    struct package_t *p;

    p = (struct package_t *)malloc(sizeof(struct package_t));
    memset(p, 0, sizeof(struct package_t));
    p->package = strdup(name);
    return p;
}

void
di_pkg_free(struct package_t *p)
{
    struct language_description *l1, *l2;
    int i;

    /* No-op if p is NULL, like free(3) */
    if (p == NULL)
        return;
    free(p->package);
    free(p->filename);
    free(p->md5sum);
    free(p->description);
    for (i = 0; p->depends[i] != NULL; i++)
    {
        free(p->depends[i]->name);
        free(p->depends[i]);
    }
    for (i = 0; p->provides[i] != NULL; i++)
    {
        free(p->provides[i]->name);
        free(p->provides[i]);
    }
    l1 = p->localized_descriptions;
    while (l1 != NULL)
    {
        l2 = l1;
        l1 = l1->next;
        free(l2->language);
        free(l2->description);
        free(l2);
    }
    free(p->version);
    free(p);
}

#define BUFSIZE         4096
struct linkedlist_t *
di_pkg_parse(FILE *f)
{
    char buf[BUFSIZE];
    struct linkedlist_t *list;
    struct list_node *node;
    struct package_t *p = NULL;
    struct package_dependency *dep;
    char *b;
    int i;

    list = (struct linkedlist_t *)malloc(sizeof(struct linkedlist_t));
    list->head = list->tail = NULL;
    while (fgets(buf, BUFSIZE, f) && !feof(f))
    {
        buf[strlen(buf)-1] = 0;
        if (di_stristr(buf, "Package: ") == buf)
        {
            p = di_pkg_alloc(strchr(buf, ' ') + 1);
            node = (struct list_node *)malloc(sizeof(struct list_node));
            node->next = NULL;
            node->data = p;
            if (list->head != NULL)
            {
                list->tail->next = node;
                list->tail = node;
            }
            else
            {
                list->head = node;
                list->tail = node;
            }
        }
	else if (di_stristr(buf, "Version: ") == buf)
        {
            p->version = strdup(strchr(buf, ' ') + 1);
        }
        else if (di_stristr(buf, "Filename: ") == buf)
        {
            p->filename = strdup(strchr(buf, ' ') + 1);
        }
        else if (di_stristr(buf, "MD5sum: ") == buf)
        {
            p->md5sum = strdup(strchr(buf, ' ') + 1);
        }
        else if (di_stristr(buf, "installer-menu-item: ") == buf)
        {
            p->installer_menu_item = atoi(strchr(buf, ' ') + 1);
        }
        else if (di_stristr(buf, "Status: ") == buf)
        {
            if (di_stristr(buf, " unpacked"))
                p->status = unpacked;
            else if (di_stristr(buf, " half-configured"))
                p->status = half_configured;
            else if (di_stristr(buf, " installed"))
                p->status = installed;
            else
                p->status = other;
        }
        else if (di_stristr(buf, "Priority: ") == buf)
        {
            if (di_stristr(buf, " extra"))
                p->priority = extra;
            else if (di_stristr(buf, " optional"))
                p->priority = optional;
            else if (di_stristr(buf, " standard"))
                p->priority = standard;
            else if (di_stristr(buf, " important"))
                p->priority = important;
            else if (di_stristr(buf, " required"))
                p->priority = required;
            else
                /* ??? */
                p->priority = extra;
        }
        else if (di_stristr(buf, "Description: ") == buf)
        {
            p->description = strdup(strchr(buf, ' ') + 1);
        }
        else if (di_stristr(buf, "Description-") == buf)
        {
            struct language_description *langdesc;
            char *colon_idx, *dash_idx;
            char *lang_code;
            int lang_code_len;
            
            dash_idx = strchr(buf, '-');
            if ((colon_idx = strstr(buf, ": ")) != NULL)
            {
                lang_code_len = colon_idx - dash_idx;
                lang_code = (char *)malloc(lang_code_len);
                memcpy(lang_code, dash_idx + 1, lang_code_len-1);
                lang_code[lang_code_len-1] = '\0';
                langdesc = malloc(sizeof(struct language_description));
                memset(langdesc, 0, sizeof(struct language_description));
                langdesc->language = lang_code;
                langdesc->description = strdup(colon_idx + 2);
                if (p->localized_descriptions) 
                    langdesc->next = p->localized_descriptions;
                p->localized_descriptions = langdesc;
            }
        }
        else if (di_stristr(buf, "Depends: ") == buf)
        {
            /*
             * Basic depends line parser. Can ignore versioning
             * info since the depends are already satisfied.
             */
            b = strchr(buf, ' ') + 1;
            i = 0;
            while (*b != 0 && *b != '\n')
            {
                if (*b != ' ' && *b != ',')
                {
                    dep = malloc(sizeof(struct package_dependency));
                    dep->name = b;
                    while (*b != 0 && *b != '\n' && *b != ',')
                    {
                        if (*b == ' ')
                            *b = 0;
                        b++;
                    }
                    *b = 0;
                    dep->name = strdup(dep->name);
                    dep->ptr = NULL;
                    p->depends[i] = dep;
                    p->depends[++i] = 0;
                    if (i == DEPENDSMAX)
                    {
                        char *emsg;

                        asprintf(&emsg, "Package %s has more than %d dependencies!",
                                p->package, DEPENDSMAX-1);
                        di_log(emsg);
                        free(emsg);
                    }
                }
                b++;
            }
        }
        else if (di_stristr(buf, "Provides: ") == buf)
        {
            b = strchr(buf, ' ') + 1;
            i = 0;
            while (*b != 0 && *b != '\n')
            {
                if (*b != ' ' && *b != ',')
                {
                    dep = malloc(sizeof(struct package_dependency));
                    dep->name = b;
                    while (*b != 0 && *b != '\n' && *b != ',')
                    {
                        if (*b == ' ')
                            *b = 0;
                        b++;
                    }
                    *b = 0;
                    dep->name = strdup(dep->name);
                    dep->ptr = NULL;
                    p->provides[i] = dep;
                    p->provides[++i] = 0;
                    if (i == PROVIDESMAX)
                    {
                        char *emsg;

                        asprintf(&emsg, "Package %s has more than %d provides!",
                                p->package, PROVIDESMAX-1);
                        di_log(emsg);
                        free(emsg);
                    }
                }
                b++;
            }
        }
    }

    return list;
}

struct package_t *
di_pkg_find(struct linkedlist_t *ptr, const char *package)
{
    struct list_node *node;
    struct package_t *p;

    for (node = ptr->head; node != NULL; node = node->next)
    {
        p = (struct package_t *)node->data;
        if (strcmp(p->package, package) == 0)
            return p;
    }
    return NULL;
}

/* Does 'ptr' provide 'target'? */
int
di_pkg_provides(struct package_t *p, struct package_t *target)
{
    int i;

    for (i = 0; p->provides[i]; i++) {
        if (p->provides[i]->ptr == target)
            return 1;
    }
    return 0;
}

int
di_pkg_is_virtual(struct package_t *p)
{
    int i;

    for (i = 0; p->depends[i] != NULL; i++)
    {
        if (p->depends[i]->ptr != NULL && di_pkg_provides(p->depends[i]->ptr, p))
            return 1;
    }
    return 0;
}

/* For a virtual package, it's true if any of its providing packages
 * is installed */
int
di_pkg_is_installed(struct package_t *p)
{
    struct package_t *q;
    int i;

    for (i = 0; p->depends[i] != NULL; i++)
    {
        q = p->depends[i]->ptr;
        if (q != NULL && di_pkg_provides(q, p) && q->status == installed)
            return 1;
    }
    return (p->status == installed);
}

/* Resolve dependency/provides pointers. Creates dummy packages for virtual
 * packages with reverse depends.
 *
 * This function has quadratic complexity over the number of packages,
 * but since the number of packages will typically be <100, we don't
 * care all that much about it. */
void
di_pkg_resolve_deps(struct linkedlist_t *ptr)
{
    struct list_node *node, *newnode;
    struct package_t *p, *q;
    struct package_dependency *dep;
    int i, j;

    /* Start by resolving provides, creating bogus packages with reverse depends */
    for (node = ptr->head; node != NULL; node = node->next)
    {
        p = (struct package_t *)node->data;
        for (i = 0; p->provides[i] != NULL; i++)
        {
            if ((q = di_pkg_find(ptr, p->provides[i]->name)) == NULL)
            {
                q = di_pkg_alloc(p->provides[i]->name);
                newnode = (struct list_node *)malloc(sizeof(struct list_node));
                newnode->data = q;
                newnode->next = ptr->head;
                ptr->head = newnode;
            }
            p->provides[i]->ptr = q;
            for (j = 0; q->depends[j] != NULL; j++)
                ;
            dep = malloc(sizeof(struct package_dependency));
            dep->name = strdup(p->package);
            dep->ptr = p;
            q->depends[j] = dep;
            q->depends[j+1] = NULL;
        }
    }

    /* Now resolve the dependencies */
    for (node = ptr->head; node != NULL; node = node->next)
    {
        p = (struct package_t *)node->data;
        for (i = 0; p->depends[i] != NULL; i++)
        {
            if (p->depends[i]->ptr == NULL)
                p->depends[i]->ptr = di_pkg_find(ptr, p->depends[i]->name);
        }
    }

    return;
}

/* visit function for the depth-first traversal performed by the
 * toposort functions */
static void
dfs_visit(struct package_t *p, struct linkedlist_t *queue)
{
    struct package_t *q;
    struct list_node *node;
    int i;

    p->processed = 1;
    for (i = 0; p->depends[i] != NULL; i++) {
        q = p->depends[i]->ptr;
        if (q == NULL)
            continue;
        if (!q->processed)
            dfs_visit(q, queue);
    }
    node = (struct list_node *)malloc(sizeof(struct list_node));
    node->data = p;
    node->next = NULL;
    if (queue->tail == NULL)
        queue->head = queue->tail = node;
    else {
        queue->tail->next = node;
        queue->tail = node;
    }
}

/* Create a topological ordering of set of packages. This will also pull in
 * their dependencies even if they're not actually in the list, because that's
 * what we want most of the time. By using a queue, we'll get the dependencies
 * before the packages that depend on them. */
struct linkedlist_t *
di_pkg_toposort_arr(struct package_t **packages, const int pkg_count)
{
    struct linkedlist_t *queue;
    int i;

    queue = (struct linkedlist_t *)malloc(sizeof(struct linkedlist_t));
    queue->head = queue->tail = NULL;
    for (i = 0; i < pkg_count; i++)
        packages[i]->processed = 0;
    for (i = 0; i < pkg_count; i++)
        if (!packages[i]->processed)
            dfs_visit(packages[i], queue);
    return queue;
}

/* Same as di_pkg_toposort_arr, but for a linked list of packages */
struct linkedlist_t *
di_pkg_toposort_list(struct linkedlist_t *list)
{
    struct linkedlist_t *ret;
    struct list_node *node;
    struct package_t **packages, *p;
    int count = 0, i = 0;

    for (node = list->head; node != NULL; node = node->next) {
        p = (struct package_t *)node->data;
        if (p != NULL)
            count++;
    }
    packages = malloc(count * sizeof(struct package_t *));
    for (node = list->head; node != NULL; node = node->next) {
        p = (struct package_t *)node->data;
        if (p != NULL)
            packages[i++] = p;
    }
    ret = di_pkg_toposort_arr(packages, count);
    free(packages);
    return ret;
}


/* Version parsing and comparison is taken from dpkg 1.10.9 */

/* assume ascii; warning: evaluates x multiple times! */
#define order(x) ((x) == '~' ? -1 \
                : isdigit((x)) ? 0 \
                : !(x) ? 0 \
                : isalpha((x)) ? (x) \
                : (x) + 256)

static int
verrevcmp(const char *val, const char *ref)
{
    if (!val)
        val = "";
    if (!ref)
        ref = "";

    while (*val || *ref) {
        int first_diff = 0;

        while ((*val && !isdigit(*val)) || (*ref && !isdigit(*ref))) {
            int vc = order(*val), rc = order(*ref);
            if (vc != rc)
                return vc - rc;
            val++;
            ref++;
        }

        while (*val == '0')
            val++;
        while (*ref == '0')
            ref++;
        while (isdigit(*val) && isdigit(*ref)) {
            if (!first_diff)
                first_diff = *val - *ref;
            val++;
            ref++;
        }
        if (isdigit(*val))
            return 1;
        if (isdigit(*ref))
            return -1;
        if (first_diff)
            return first_diff;
    }
    return 0;
}


int
di_parse_version(struct version_t *rversion, const char *string)
{
    char *hyphen, *colon, *eepochcolon;
    const char *end, *ptr;
    unsigned long epoch;

    if (!*string)
        return 0;

    /* trim leading and trailing space */
    while (*string && (*string == ' ' || *string == '\t'))
        string++;
    /* string now points to the first non-whitespace char */
    end = string;
    /* find either the end of the string, or a whitespace char */
    while (*end && *end != ' ' && *end != '\t')
        end++;
    /* check for extra chars after trailing space */
    ptr = end;
    while (*ptr && (*ptr == ' ' || *ptr == '\t'))
        ptr++;
    if (*ptr)
        return 0;

    colon = strchr(string,':');
    if (colon) {
        epoch = strtoul(string, &eepochcolon, 10);
        if (colon != eepochcolon)
            return 0;
        if (!*++colon)
            return 0;
        string = colon;
        rversion->epoch = epoch;
    } else {
        rversion->epoch = 0;
    }
    rversion->version = strndup(string, end-string);
    hyphen = strrchr(rversion->version, '-');
    if (hyphen)
        *hyphen++ = 0;
    rversion->revision = hyphen ? hyphen : "";

    return 1;
}


int
di_compare_version(const struct version_t *a, const struct version_t *b)
{
    int r;

    if (a->epoch > b->epoch)
        return 1;
    if (a->epoch < b->epoch)
        return -1;
    r = verrevcmp(a->version, b->version);
    if (r != 0)
        return r;
    return verrevcmp(a->revision, b->revision);
}

void
di_list_free(struct linkedlist_t *list, void (*freefunc)(void *))
{
    struct list_node *node, *next;

    if (list == NULL)
        return;
    for (node = list->head; node != NULL; node = next)
    {
        next = node->next;
        freefunc(node->data);
        free(node);
    }
    free(list);
}

/* Returns the devfs path name normalized into a "normal" (hdaX, sdaX)
 * name.  The return value is malloced, which means that the caller
 * needs to free it.  This is kinda hairy, but I don't see any other
 * way of operating with non-devfs systems when we use devfs on the
 * boot medium.  The numbers are taken from devices.txt in the
 * Documentation subdirectory off the kernel sources.
 */
char *
di_mapdevfs(const char *path)
{
/* taken from devices.txt in the kernel source, order is important,
 large before smaller. */
    static struct diskentry {
        int major;
        int minor;
        char *name;
    } diskentries[] = {
        {91,  64, "hdt"}, {91,   0, "hds"},
        {90,  64, "hdr"}, {90,   0, "hdq"},
        {89,  64, "hdp"}, {89,   0, "hdo"},
        {88,  64, "hdn"}, {88,   0, "hdm"},
        {65, 240, "sdaf"}, {65, 224, "sdae"},
        {65, 208, "sdad"}, {65, 192, "sdac"},
        {65, 176, "sdab"}, {65, 160, "sdaa"},
        {65, 176, "sdz"}, {65, 160, "sdy"},
        {65, 144, "sdx"}, {65, 112, "sdw"},
        {65,  96, "sdv"}, {65,  80, "sdu"},
        {65,  64, "sdt"}, {65,  32, "sds"},
        {65,  16, "sdr"}, {65,   0, "sdq"},
        {57,  64, "hdl"}, {57,   0, "hdk"},
        {56,  64, "hdj"}, {56,   0, "hdi"},
        {34,  64, "hdh"}, {34,   0, "hdg"},
        {33,  64, "hdf"}, {33,   0, "hde"},
        {22,  64, "hdd"}, {22,   0, "hdc"},
        { 8, 240, "sdp"}, { 8, 224, "sdo"},
        { 8, 208, "sdn"}, { 8, 192, "sdm"},
        { 8, 176, "sdl"}, { 8, 160, "sdk"},
        { 8, 176, "sdj"}, { 8, 160, "sdi"},
        { 8, 144, "sdh"}, { 8, 112, "sdg"},
        { 8,  96, "sdf"}, { 8,  80, "sde"},
        { 8,  64, "sdd"}, { 8,  32, "sdc"},
        { 8,  16, "sdb"}, { 8,   0, "sda"},
        { 3,  64, "hdb"}, { 3,   0, "hda"},
        { 2,   1, "fd1"}, { 2,   0, "fd0"},
        { 0,   0,  NULL}
    };
    int ret;
    struct stat statbuf;
    struct diskentry *de;

    char *retval = NULL;

    int partnum;

    if (0 == strcmp("none", path))
	return strdup(path);

    if (strlen(path) > 9 && 0 == strncmp("/dev/tts/", path, 9)) {
        retval = strdup(path);
        *(retval+7) = 'y'; *(retval+8) = 'S';
        return retval;
    }

    ret = stat(path,&statbuf);
    if (ret == -1)
    {
        return NULL;
    }
    
    de = diskentries;
    while (de->name != NULL) {
        if (major(statbuf.st_rdev) == de->major && 
            minor(statbuf.st_rdev) >= de->minor) {
            break;
        }
        de++;
    }
    if ( ! de->name )
    { /* Pass unknown devices on without changes.  This fixes LVM devices */
        retval = strdup(path);
        return retval;
    }
    partnum = minor(statbuf.st_rdev) - de->minor;

    if (0 == partnum)
        asprintf(&retval, "/dev/%s", de->name);
    else
        asprintf(&retval, "/dev/%s%d", de->name, partnum);

    return retval;
}

