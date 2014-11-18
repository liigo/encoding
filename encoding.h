#ifndef __TOPSERVER_ENCODING__
#define __TOPSERVER_ENCODING__

// https://github.com/liigo/encoding
// by Liigo, 2013-2014.

// Decode text that encoded by enconding.
// encoding: now supporting "%utf8", "%gb", "%u", "wchars", "gb"
// note: "%gb", "gb" and "wchars" are supported only on Windows platform.
const char* decode_to_utf8(const char* pArg, const char* encoding);

// Convert a Unicode codepoint to utf-8 encoded char, arg utf8 should be at least 4 chars buffer.
// return the bytes count used in utf-8.
int Codepoint_to_UTF8(unsigned int codepoint, char* utf8);

// Convert an utf-8 encoded stream to an Unicode codepoint, along with utf-8 validation.
// return codepoint, and bytes count used in utf8 if bytes != NULL.
// return -1 if met invalid utf-8 stream.
int UTF8_to_Codepoint(const char* utf8, int* bytes);

// from specified position, find the last leading-byte of a utf-8 encoded character,
// returns its index in buf, or returns 0 if not find.
int rfind_utf8_leading_byte_index(char* buf, int from);

#endif //__TOPSERVER_ENCODING__
