#include "pch.h"
#include "pvz.h"

// ��ȡָ���ڴ��ַ���ĸ��ֽڵ�ֵ
uint32_t R4(int value) {
    // ��intֵת��Ϊָ��
    int v = *reinterpret_cast<int*>(value);

    // ����ָ��ָ����ڴ��ַ��ֵ
    return v;
}

// д���ĸ��ֽڵ�ֵ��ָ���ڴ��ַ
void W4(void* address, uint32_t value) {
    if (address != nullptr) {
        // ����ַǿ��ת��Ϊ�޷�������ָ�룬��д���ĸ��ֽڵ�ֵ
        *reinterpret_cast<uint32_t*>(address) = value;
    }
}

template <typename T>
std::string hex(T number) {
  std::stringstream ss;
  ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << number;
  return ss.str();
}

template <class T>
void _print(T arg) {
    std::cout << arg << " ";
}

template <typename... Args>
void log(Args... args) {
    time_t t;
    struct tm tmp;
    char dt[50];
    time(&t);
    localtime_s(&tmp, &t);
    strftime(dt, sizeof(dt), "[%Y-%m-%d %H:%M:%S]", &tmp);

    std::cout << dt << " ";  // Print timestamp to console

    //std::string path = fs::current_path().string() + "/log.txt";
    std::string path = "E:/BaiduNetdiskDownload/hack/code/inject-dll/Debug/log.txt";
    // Write to log file
    std::ofstream file(path, std::ios_base::app | std::ios::out); // ��utf-8�����ʽ���ļ�
    if (file.is_open()) {
        file << dt << " ";                                               // Write timestamp
        int _[] = {0, ((_print(args), file << args << " "), 0)...};  // Expand args
        file << std::endl;                                               // End of log entry
        file.close();                                                    // Close file
    }
}

void Log(const char* format, ...) {
    const char* filename = "E:/BaiduNetdiskDownload/hack/code/inject-dll/Debug/log.txt";
    std::ofstream outputFile(filename, std::ios::app);

    va_list args;
    va_start(args, format);

    char buffer[256];  // ����һ���㹻��Ļ�����
    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    // ��ȡ��ǰʱ��
    std::time_t now = std::time(nullptr);
    struct tm localTime;
    if (localtime_s(&localTime, &now) == 0) {
        // ��ʽ��ʱ���
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S] ", &localTime);

        // �ϲ�ʱ�������־����
        std::string logMessage = std::string(timestamp) + std::string(buffer);

        if (buffer[strlen(buffer) - 1] != '\n') {
            strcat_s(buffer, "\n");
        }

        if (outputFile.is_open()) {
            outputFile << logMessage << std::endl;
            outputFile.close();
        } else {
            MessageBoxA(0, "��־д��ʧ��", "��ʾ", MB_OK);
        }
    } else {
        MessageBoxA(0, "��ȡ����ʱ��ʧ��", "��ʾ", MB_OK);
    }
}

bool ApplyHook(LPVOID HookAddress, const char* HookCode, size_t codeLength) {
    DWORD Protect = 0;
    if (HookAddress != NULL) {
        if (VirtualProtect(HookAddress, codeLength, PAGE_EXECUTE_READWRITE, &Protect)) {
            memcpy(HookAddress, HookCode, codeLength);
            VirtualProtect(HookAddress, codeLength, Protect, &Protect);
            return true;
        }
    }

    return false;
}

Pvz::Pvz() {
    // ��ַ 0x00 43 CC 6E
    // Ҫ�޸ĵ�ֵ 80 7B 50 01
    HookAddress = 0x0043CC6E;
    init();
}

void Pvz::init() {
    unsigned char initialHookCode1[] = {0x80, 0x7B, 0x50, 0x01};
    memcpy(HookCode, initialHookCode1, sizeof(HookCode));
    unsigned char initialHookCode2[] = {0x80, 0x7B, 0x50, 0x00};
    memcpy(UnHookCode, initialHookCode2, sizeof(UnHookCode));
}

BOOL Pvz::OpenAutoGetSun() {
    if (ApplyHook((LPVOID)HookAddress, HookCode, 4)) {
        return TRUE;
    }
    return FALSE;
}

BOOL Pvz::CloseAutoGetSun() {
    if (ApplyHook((LPVOID)HookAddress, UnHookCode, 4)) {
        return TRUE;
    }
    return FALSE;
}

BOOL Pvz::AddSun(int SunNumber) {
    //[[755EAC]+868]+5578
    int SunAddress = R4(R4(0x755EAC) + 0x868) + 0x5578;
    int Sun = R4(SunAddress);

    if (Sun) {
        W4((void*)SunAddress, Sun + SunNumber);
        return TRUE;
    }
    return FALSE;
}

// ȫ����ֲ
void Pvz::FullScreenPlant(int PlantId) {
    //__asm {
    //    pushad
    //    pushfd
    //    mov ecx,dword ptr ds:[0x00755EAC]
    //    mov ecx,[ecx+0x868]
    //    push 0xFFFFFFFF
    //    push 3
    //    mov eax,2
    //    push 2
    //    push ecx
    //    mov esi,0x00418D70
    //    call esi
    //    popfd
    //    popad
    //}
    log("��ֲֲ��id", PlantId);
    for (size_t x = 0; x <= 8; x++) {
        for (size_t y = 0; y <= 5; y++) {
            __asm {
                //pushad
                mov ecx,dword ptr ds:[0x00755EAC]
                mov ecx,[ecx+0x868]
                push 0xFFFFFFFF
                push PlantId  // ֲ��id  
                mov eax,y  // y
                push x  // x
                push ecx
                mov esi,0x00418D70
                call esi
                //popad
            }
        }
    }
}

//����ֲ�￨Ƭ���Ե��
BOOL Pvz::OpenCardClickable() {
    log("OpenCardClickable");

    unsigned char HookCode[] = {0xC6, 0x43, 0x48, 0x01};
    if (ApplyHook((LPVOID)0x0049E944, (char*)HookCode, 4)) {
        return TRUE;
    }

    return FALSE;
}

BOOL Pvz::CloseCardClickable() {
    unsigned char UnHookCode[] = {0xC6, 0x43, 0x48, 0x00};

    if (ApplyHook((LPVOID)0x0049E944, (char*)UnHookCode, 4)) {
        return TRUE;
    }
    return FALSE;
}

//������ȴ��ַ
//PlantsVsZombies.exe+6DE0E  0046DE18

//ֲ����ֲcall
//push ecx
//mov ecx,dword ptr ds:[0x00755EAC]
//mov ecx,[ecx+0x868]
//push 0xFFFFFFFF
//push 3
//mov eax,2
//push 2
//push ecx
//mov esi,0x00418D70
//call esi
//pop ecx