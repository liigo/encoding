#ifndef __TOPSERVER_ENCODING__
#define __TOPSERVER_ENCODING__

// Decode text that encoded by enconding.
// encoding: now supporting "%utf8", "%gb", "%u", "wchars", "gb"
const char* decode_to_utf8(const char* pArg, const char* encoding);

// Convert a Unicode codepoint to utf-8 encoded char, arg utf8 should be at least 4 chars buffer, best for 6 chars.
// return the bytes count used in utf-8.
int Codepoint_to_UTF8(unsigned int codepoint, char* utf8);

// Convert a utf-8 encoded char to Unicode codepoint.
// return codepoint, and bytes count used in utf-8 if bytes != NULL.
unsigned int UTF8_to_Codepoint(const char* utf8, int* bytes);


#endif //__TOPSERVER_ENCODING__
