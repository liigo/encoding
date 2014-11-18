encoding
========
to convert between unicode codepoint and utf-8 bytes, to decode "percent encoding" string to utf-8.

by Liigo, Oct. 2013 - Nov. 2014.

###How to use
See encoding.h header:

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

and the test.c for testing and usage:

	void test(const char* text, const char* encoding, const char* file)
	{
		writefile(decode_to_utf8(text, encoding), file);
	}

	int main()
	{
		// Liigo���
		test("Liigo%C4%E3%BA%C3", "%gb", "1.txt");
		// Liigo����
		test("Liigo%E9%AB%98%E6%89%8B", "%utf8", "2.txt");
		// "Liigo���ϴ� �� ��̳�� ������ ����ɾ��  �Q��"
		test("Liigo%u662F%u8001%u5927 %u4e8e %u8bba%u575B%u4e0A %u6c42%u8d34%u89e3 %u90Fd%u88aB%u5220%u6389  %u9f49%u7228", "%u", "3.txt");
		// ����4, ��+��+�磨�����½ṹ�����޺ᣩ�����������Ƿǳ����ӵĺ��֣���Unicode��׼�����¼���ģ�δ��������֧������ʾ��
		test("Liigo%U0002A6A5%U0002B81D", "%u", "4.txt");

		{
			int bytes;
			int codepoint = 0;
			unsigned char utf8[] = { 0xe6,0x98,0xaf, 0xe8,0x80,0x81, 0xe5,0xa4,0xa7, // ���ϴ�
							0xe4,0xba,0x8e, 0xe8,0xae,0xba, 0xe5,0x9d,0x9b, // ����̳
							0xe9,0xbd,0x89, 0xe7,0x88,0xa8, // �Q��
							0xF0,0xAA,0x9A,0xA5, 0xF0,0xAB,0xA0,0x9D, // ����4, ��+��+�磨�����½ṹ�����޺ᣩ��ÿ��UTF-8������4�ֽ�
							0x00, };

			codepoint = UTF8_to_Codepoint(utf8 + 0, &bytes); assert(codepoint==0x662F && bytes==3); // ��
			codepoint = UTF8_to_Codepoint(utf8 + 3, &bytes); assert(codepoint==0x8001 && bytes==3); // ��
			codepoint = UTF8_to_Codepoint(utf8 + 6, &bytes); assert(codepoint==0x5927 && bytes==3); // ��
			codepoint = UTF8_to_Codepoint(utf8 + 9, &bytes); assert(codepoint==0x4e8e && bytes==3); // ��
			codepoint = UTF8_to_Codepoint(utf8 +12, &bytes); assert(codepoint==0x8bba && bytes==3); // ��
			codepoint = UTF8_to_Codepoint(utf8 +15, &bytes); assert(codepoint==0x575b && bytes==3); // ̳
			codepoint = UTF8_to_Codepoint(utf8 +18, &bytes); assert(codepoint==0x9f49 && bytes==3); // �Q
			codepoint = UTF8_to_Codepoint(utf8 +21, &bytes); assert(codepoint==0x7228 && bytes==3); // ��
			codepoint = UTF8_to_Codepoint(utf8 +24, &bytes); assert(codepoint==0x0002A6A5 && bytes==4); // ����4
			codepoint = UTF8_to_Codepoint(utf8 +28, &bytes); assert(codepoint==0x0002B81D && bytes==4); // ��+��+�磨�����½ṹ�����޺ᣩ
			
			// test invalid utf8 stream
			utf8[0] = 0xFF; codepoint = UTF8_to_Codepoint(utf8 + 0, NULL); assert(codepoint == -1);
			utf8[4] = 0x00; codepoint = UTF8_to_Codepoint(utf8 + 3, NULL); assert(codepoint == -1);
			utf8[8] = 0x00; codepoint = UTF8_to_Codepoint(utf8 + 6, NULL); assert(codepoint == -1);
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

Have a good day!
