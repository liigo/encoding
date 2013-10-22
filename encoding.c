#include "encoding.h"

#include <Windows.h>

// CharHexMap[char]
static int CharHexMap[] = {
	-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, //  0-19
	-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, // 20-39
	-1,-1,-1,-1,-1, -1,-1,-1, 0, 1,   2, 3, 4, 5, 6,  7, 8, 9,-1,-1, // 40-59 (48-57: 0..9)
	-1,-1,-1,-1,-1, 10,11,12,13,14,  15,-1,-1,-1,-1, -1,-1,-1,-1,-1, // 60-79 (65-70: A..F)
	-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,  -1,-1,-1,-1,-1, -1,-1,10,11,12, // 80-99 (97-102: a..f)
	13,14,15,-1,-1, -1,-1,-1,-1,-1,  -1,-1,-1,-1,-1, -1,-1,-1,-1,-1, // 100-119
	-1,-1,-1,-1,-1, -1,-1,-1,-1,                                     // 120-128
};

// Convert a Unicode codepoint to utf-8 encoded char, arg utf8 should be at least 3 chars buffer, best for 6 chars.
// return the bytes count used in utf-8.
int Codepoint_to_UTF8(unsigned int codepoint, char* utf8)
{
	if(codepoint <= 0x7F) {
		utf8[0] = (char)codepoint;
		return 1;
	} else if(codepoint <= 0x07FF) {
		utf8[0] = (char) (0x90 | (codepoint >> 6));   // 110xxxxx
		utf8[1] = (char) (0x80 | (codepoint & 0x3F)); // 10xxxxxx
		return 2;
	} else if(codepoint <= 0xD7FF || (codepoint >= 0xE000 && codepoint <= 0xFFFF)) {
		utf8[0] = (char) (0xE0 | (codepoint >> 12));         // 1110xxxx
		utf8[1] = (char) (0x80 | ((codepoint >> 6) & 0x3F)); // 10xxxxxx
		utf8[2] = (char) (0x80 | (codepoint & 0x3F));        // 10xxxxxx
		return 3;
	}
	return 0;
}

// Convert a utf-8 encoded char to Unicode codepoint.
// return codepoint, and bytes count used in utf-8 if bytes != NULL.
unsigned int UTF8_to_Codepoint(const char* utf8, int* bytes)
{
	unsigned char b1 = (unsigned char) *utf8;
	if((b1 >> 7) == 0) { // 0xxxxxxx
		if(bytes) *bytes = 1;
		return b1;
	} else if((b1 >> 5) == 0x06) { // 110xxxxx 10xxxxxx
		unsigned char b2 = (unsigned char) utf8[1];
		if(bytes) *bytes = 2;
		return (((unsigned int)b1 & 0x1F) << 6) | (b2 & 0x3F);
	} else if((b1 >> 4) == 0x0E) { // 1110xxxx 10xxxxxx 10xxxxxx
		unsigned char b2 = (unsigned char) utf8[1];
		unsigned char b3 = (unsigned char) utf8[2];
		if(bytes) *bytes = 3;
		return (((unsigned int)b1 & 0x0F) << 12) | (((unsigned int)b2 & 0x3F) << 6) | (b3 & 0x3F);
	}
	return 0;
}

// %AB%AB%AB ->> ABABAB
// Need free() returned value
static const char* eval_percent_chars(const char* pArg)
{
	const char* p = pArg;
	char *pReturn, *pr;
	if(pArg == NULL || *pArg == '\0') return NULL;
	pReturn = (char*) malloc(strlen(pArg));
	if(pReturn == NULL) return NULL;
	pr = pReturn;

	while(*p)
	{
		if(*p == '%')
		{
			if(p[1] > 0 && p[2] == '%') { // %A
				*pr++ = (char)CharHexMap[p[1]];
				p += 2;
				continue;
			} else if((p[1] == 'u' || p[1] == 'U') && p[2] > 0 && p[3] > 0 && p[4] > 0 && p[5] > 0) { // %uABCD
				int a = CharHexMap[p[2]], b = CharHexMap[p[3]];
				int c = CharHexMap[p[4]], d = CharHexMap[p[5]];
				if(a >= 0 && b >= 0 && c >= 0 && d >= 0) {
					unsigned int codepoint = ((((unsigned char)a << 4) | b) << 8) | (((unsigned char)c << 4) | d);
					int bytes = Codepoint_to_UTF8(codepoint, pr);
					pr += bytes;
				}
				p += 6;
				continue;
			} else if(p[1] > 0 && p[2] > 0) { // %AB
				int a = CharHexMap[p[1]], b = CharHexMap[p[2]];
				if(a >=0 && b >= 0)
					*pr++ = (char)(((unsigned char)a << 4) + b);
				p += 3;
				continue;
			}
		} else {
			*pr++ = *p;
		}
		p++;
	}
	*pr = '\0';
	return pReturn;
}

// Need free() returned value
const WCHAR* GB18030_to_WChars(const char* pGB)
{
	int gblen, wlen;
	WCHAR* pReturn;
	if(pGB == NULL || *pGB == '\0') return NULL;
	gblen = strlen(pGB);
	wlen = MultiByteToWideChar(CP_ACP, 0, pGB, gblen, NULL, 0);
	if(wlen == 0) return NULL;
	pReturn = (WCHAR*) malloc(sizeof(WCHAR) * (wlen + 1));
	if(pReturn == NULL) return NULL;
	wlen = MultiByteToWideChar(CP_ACP, 0, pGB, gblen, pReturn, wlen + 1);
	if(wlen == 0) {
		free((char*)pReturn);
		return NULL;
	} else {
		pReturn[wlen] = L'\0';
		return (const WCHAR*) pReturn;
	}
}

// Need free() returned value
const char* WChars_to_UTF8(const WCHAR* pWChars)
{
	int wlen, ulen;
	char* pReturn;
	if(pWChars == NULL || *pWChars == '\0') return NULL;
	wlen = wcslen(pWChars);
	ulen = WideCharToMultiByte(CP_UTF8, 0, pWChars, wlen, NULL, 0, NULL, NULL);
	if(ulen == 0) return NULL;
	pReturn = (char*) malloc((ulen + 1));
	if(pReturn == NULL) return NULL;
	ulen = WideCharToMultiByte(CP_UTF8, 0, pWChars, wlen, pReturn, ulen + 1, NULL, NULL);
	if(ulen == 0) {
		free(pReturn);
		return NULL;
	} else {
		pReturn[ulen] = '\0';
		return (const char*) pReturn;
	}
}

// %AB%AB%AB ->> ABABAB
// Need free() returned value
const char* Percent_UTF8_to_UTF8(const char* pArg)
{
	return eval_percent_chars(pArg);
}

// %AB%AB ->> ...
// Need free() returned value
const char* Percent_GB18030_to_UTF8(const char* pArg)
{
	WCHAR* pWChars;
	const char* pGB = eval_percent_chars(pArg);
	if(pGB == NULL) return NULL;
	pWChars = (WCHAR*) GB18030_to_WChars(pGB);
	free((char*)pGB);
	if(pWChars) {
		const char* utf8chars = WChars_to_UTF8(pWChars);
		free((char*)pWChars);
		return utf8chars;
	}
	return NULL;
}

// %uABCD ->> ...
// Need free() returned value
const char* Percent_Unicode_to_UTF8(const char* pArg)
{
	return eval_percent_chars(pArg);
}

// Need free() returned value
const char* decode_to_utf8(const char* pArg, const char* encoding)
{
	if(pArg == NULL || encoding == NULL) return NULL;

	if(strcmp(encoding, "%utf8") == 0)
		return Percent_UTF8_to_UTF8(pArg);
	else if(strcmp(encoding, "%gb") == 0)
		return Percent_GB18030_to_UTF8(pArg);
	else if(strcmp(encoding, "%u") == 0)
		return Percent_Unicode_to_UTF8(pArg);
	else if(strcmp(encoding, "wchars") == 0)
		return WChars_to_UTF8((const WCHAR*)pArg);
	else if(strcmp(encoding, "gb") == 0) {
		const WCHAR* pWChars = GB18030_to_WChars(pArg);
		if(pWChars) {
			const char* utf8chars = WChars_to_UTF8(pWChars);
			free((char*)pWChars);
			return utf8chars;
		}
	}
	return NULL;
}
