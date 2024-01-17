/* $Id$ */
#ifndef __W3DNS_H__
#define __W3DNS_H__

#include <stdint.h>
#include <stdbool.h>

int __W3_DNS_Connect(const char* hostname, bool ssl, uint16_t port);

#endif
