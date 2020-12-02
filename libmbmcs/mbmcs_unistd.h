#ifndef INC_MBMCS_UNISTD_H
#define INC_MBMCS_UNISTD_H

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int64_t mbmcs_sysconf_pagesize(void);
int64_t mbmcs_sysconf_nprocessor_conf(void);
int64_t mbmcs_sysconf_nprocessor_onln(void);
int64_t mbmcs_sysconf_phys_pages(void);
int64_t mbmcs_sysconf_avphys_pages(void);

#ifdef __cplusplus
}
#endif

#endif /* end of INC_MBMCS_UNISTD_H */
