/* $Id$ */
#ifndef __W3VERSION_H__
#define __W3VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LIBW3_VERSION "2.16C" \
SUFFIX

ifdef(`HTTP_SUPPORT', `#define LIBW3_HTTP_SUPPORT', `')
ifdef(`FTP_SUPPORT', `#define LIBW3_FTP_SUPPORT', `')
ifdef(`GOPHER_SUPPORT', `#define LIBW3_GOPHER_SUPPORT', `')
ifdef(`GEMINI_SUPPORT', `#define LIBW3_GEMINI_SUPPORT', `')
ifdef(`FINGER_SUPPORT', `#define LIBW3_FINGER_SUPPORT', `')
ifdef(`NEX_SUPPORT', `#define LIBW3_NEX_SUPPORT', `')
ifdef(`FILE_SUPPORT', `#define LIBW3_FILE_SUPPORT', `')

#ifdef __cplusplus
}
#endif

#endif
