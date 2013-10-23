encoding
========
to decode "percent encoding" string to utf-8, to convert between unicode codepoint and utf-8 chars.

by Liigo, Oct. 2013.

###How to use
See encoding.h header:

	// Decode text that encoded by enconding.
	// encoding: now supporting "%utf8", "%gb", "%u", "wchars", "gb"
	const char* decode_to_utf8(const char* pArg, const char* encoding);

	// Convert a Unicode codepoint to utf-8 encoded char, arg utf8 should be at least 3 chars buffer, best for 6 chars.
	// return the bytes count used in utf-8.
	int Codepoint_to_UTF8(unsigned int codepoint, char* utf8);

	// Convert a utf-8 encoded char to Unicode codepoint.
	// return codepoint, and bytes count used in utf-8 if bytes != NULL.
	unsigned int UTF8_to_Codepoint(const char* utf8, int* bytes);

and the test.c for testing and usage:

	void test(const char* text, const char* encoding, const char* file)
	{
		writefile(decode_to_utf8(text, encoding), file);
	}

	int main()
	{
		// Liigo你好
		test("Liigo%C4%E3%BA%C3", "%gb", "1.txt");
		// Liigo高手
		test("Liigo%E9%AB%98%E6%89%8B", "%utf8", "2.txt");
		// "Liigo是老大 于 论坛上 求贴解 都被删掉  齉爨"
		test("Liigo%u662F%u8001%u5927 %u4e8e %u8bba%u575B%u4e0A %u6c42%u8d34%u89e3 %u90Fd%u88aB%u5220%u6389  %u9f49%u7228", "%u", "3.txt");

		{
			int bytes;
			unsigned int codepoint = 0;
			unsigned char utf8[] = { 0xe6,0x98,0xaf, 0xe8,0x80,0x81, 0xe5,0xa4,0xa7, // 是老大
							0xe4,0xba,0x8e, 0xe8,0xae,0xba, 0xe5,0x9d,0x9b, // 于论坛
							0xe9,0xbd,0x89, 0xe7,0x88,0xa8, // 齉爨
							0x00, };

			codepoint = UTF8_to_Codepoint(utf8 + 0, &bytes); // codepoint: 0x662F, bytes: 3
			codepoint = UTF8_to_Codepoint(utf8 + 3, &bytes); // codepoint: 0x8001, bytes: 3
			codepoint = UTF8_to_Codepoint(utf8 + 6, &bytes); // codepoint: 0x5927, bytes: 3
			codepoint = UTF8_to_Codepoint(utf8 + 9, &bytes); // codepoint: 0x4e8e, bytes: 3
			codepoint = UTF8_to_Codepoint(utf8 +12, &bytes); // codepoint: 0x8bba, bytes: 3
			codepoint = UTF8_to_Codepoint(utf8 +15, &bytes); // codepoint: 0x575b, bytes: 3
			codepoint = UTF8_to_Codepoint(utf8 +18, &bytes); // codepoint: 0x9f49, bytes: 3
			codepoint = UTF8_to_Codepoint(utf8 +21, &bytes); // codepoint: 0x7228, bytes: 3
			codepoint = 0;
		}
	}

	void writefile(const char* data, const char* file)
	{
		FILE* f = fopen(file, "wb+");
		if(f) {
			fwrite(data, 1, strlen(data), f);
			fclose(f);
		}
	}

Have a good day.
