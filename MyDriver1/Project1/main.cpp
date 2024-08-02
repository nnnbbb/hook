#include <Windows.h>
#include <iostream>

int main() {
	DWORD32 testVal = 111;
	HANDLE h = GetCurrentProcess();
    DWORD32 t;
    size_t len;
    ReadProcessMemory(h, &testVal, &t, 4, &len);


	HANDLE handle = CreateFileA("\\\\.\\MyDriver", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (handle == INVALID_HANDLE_VALUE) {
		return -1;
	}
	printf("Successfully Open MyDriver£¡\r\n");
	const char* s = "hello!";
	DWORD written = 0;
	WriteFile(handle, s, strlen(s) + 1, &written, 0);

	char readBuffer[0x100] = { 0 };
	LPDWORD readSize = 0;

	ReadFile(handle, readBuffer, 0x100, readSize, 0);
	printf("Receive r0 data %s\r\n", readBuffer);

	CloseHandle(handle);
	system("pause");
	return 0;
}