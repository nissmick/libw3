/* $Id$ */
#ifndef __W3DNS_H__
#define __W3DNS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

int __W3_DNS_Connect(const char* hostname, bool ssl, uint16_t port
#ifdef SSL_SUPPORT
	,
	void** o_ssl,
	void** o_ctx
#endif
);

#ifdef __cplusplus
}
#endif

#endif
