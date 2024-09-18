#include "plugin.h"
#include "parser.h"
#include <keystone/keystone.h>
#include <sstream>

enum
{
	MENU_DISASM_PASTE,
	MENU_DISASM_PASTE_PATCH,
	MENU_DUMP_PASTE,
	MENU_DUMP_PASTE_PATCH,
	MENU_DISASM_ABOUT,
	MENU_DUMP_ABOUT,
	MENU_DISASM_PASTE_ASSEBLY,
	MENU_DUMP_PASTE_ASSEBLY,
};


#define ABOUT_MSG "yummyPaste by Oguz Kartal\r\n\r\n" \
				  "paste your shellcode string into the x64dbg!\r\n\r\nhttps://oguzkartal.net\r\n" \
				  "compiled in: "  __DATE__ " " __TIME__

#define Cmd(x) DbgCmdExecDirect(x)
#define Eval(x) DbgValFromString(x)
#define PLUG_EXPORT extern "C" __declspec(dllexport)
#define dputs(x) _plugin_logprintf("[" PLUGIN_NAME "] %s\n", x)

#ifdef DEBUG_PREFIX
#define dprintf(x, ...) _plugin_logprintf("[" PLUGIN_NAME "] " x, __VA_ARGS__)
#else
#define dprintf(x, ...) _plugin_logprintf(x, __VA_ARGS__)
#endif // DEBUG_PREFIX


void About()
{
	MessageBoxA(hwndDlg, ABOUT_MSG, "yummyPaste", MB_ICONINFORMATION);
}

LPSTR GetClipboardTextData(size_t* pLength)
{
	LPSTR temp = NULL, pastedContent = NULL;
	size_t contentLength = 0;
	HANDLE cbHandle = NULL;

	if (pLength)
		*pLength = 0;

	if (!OpenClipboard(hwndDlg))
	{
		MessageBoxA(hwndDlg, "The clipboard couldn't be opened", "yummyPaste", MB_ICONWARNING);
		goto oneWayExit;
	}


	if (!IsClipboardFormatAvailable(CF_TEXT))
	{
		goto oneWayExit;
	}

	cbHandle = GetClipboardData(CF_TEXT);

	if (!cbHandle)
	{
		MessageBoxA(hwndDlg, "Clipboard data couldn't readed", "yummyPaste", MB_ICONWARNING);
		goto oneWayExit;
	}



	temp = (LPSTR)GlobalLock(cbHandle);

	if (!temp)
	{
		MessageBoxA(hwndDlg, "The data couldn't be extracted from the cb object", "yummyPaste", MB_ICONSTOP);
		goto oneWayExit;
	}

	contentLength = strlen(temp);

	if (contentLength == 0)
		goto oneWayExit;

	pastedContent = (LPSTR)Malloc(contentLength + 1);


	if (!pastedContent)
	{
		goto oneWayExit;
	}

	strcpy_s(pastedContent, contentLength + 1, temp);


	if (pLength)
		*pLength = contentLength;

oneWayExit:

	if (temp)
	{
		GlobalUnlock(cbHandle);
		temp = NULL;
	}

	CloseClipboard();

	return pastedContent;
}



void MakeTomatoPaste(int window, BOOL patched)
{
	size_t pdLen = 0;
	LPSTR pasteData = NULL;
	SELECTIONDATA sel = { 0 };
	BINARY_DATA* binary = NULL;

	if (!DbgIsDebugging())
	{
		MessageBoxA(hwndDlg, "Where is your tomato to be paste?", "yummyPaste", MB_ICONWARNING);
		return;
	}

	GuiSelectionGet(window, &sel);


	pasteData = GetClipboardTextData(&pdLen);

	if (!pasteData)
		return;

	ResetBinaryObject();

	ParseBytes(pasteData, pdLen);

	binary = GetBinaryData();

	if (binary->invalid)
	{
		MessageBoxA(hwndDlg, "looks like your content is invalid to me", "yummyPaste", MB_ICONWARNING);
		Free(pasteData);
		return;
	}

	if (patched)
		DbgFunctions()->MemPatch(sel.start, binary->binary, binary->index);
	else
		DbgMemWrite(sel.start, binary->binary, binary->index);

	Free(pasteData);

	if (window == GUI_DISASSEMBLY)
		GuiUpdateDisassemblyView();
	else if (window == GUI_DUMP)
		GuiUpdateDumpView();

}


void PrintAscii(const char* asm_str) {

	dprintf("PrintAscii\n");
	for (const char* ptr = asm_str; *ptr != '\0'; ++ptr) {
		dprintf("%c = %d\n", *ptr, static_cast<int>(*ptr));
	}
}

int GetAssemblyHex(const char* asm_str, unsigned char** encode_ptr, size_t* size_ptr) {
	// PrintAscii(asm_str);
	ks_engine* ks;
	ks_err err = ks_err::KS_ERR_OK;
	size_t count;

	if ((err = ks_open(KS_ARCH_X86, KS_MODE_32, &ks)) != KS_ERR_OK) {
		dprintf("Failed to initialize Keystone Engine");
		return 1;
	}

	if ((err = (ks_err)ks_asm(ks, asm_str, 0, encode_ptr, size_ptr, &count)) != KS_ERR_OK) {
		dprintf("Failed to assemble given code = %s\n", ks_strerror((ks_err)err));
		dprintf("Failed to assemble given asm_str = %s\n", asm_str);
		return 1;
	}

	ks_close(ks);

	return 0;
}

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Move past the to string
	}
	return str;
}

std::string removeSegmentRegisters(const std::string& input) {
	std::string result = input;
	result = replaceAll(result, "ss:", "");
	result = replaceAll(result, "ds:", "");
	result = replaceAll(result, "es:", "");
	result = replaceAll(result, "fs:", "");
	result = replaceAll(result, "gs:", "");
	return result;
}

// 处理输入的函数
void ProcessInput(const std::string& input, std::vector<std::string>& str) {
	std::string temp = input;
	size_t pos = 0;

	while ((pos = temp.find("\r\n")) != std::string::npos) {
		std::string s = temp.substr(0, pos);
		// 使用 removeSegmentRegisters 函数替换段寄存器
		s = removeSegmentRegisters(s);
		str.push_back(s);
		temp.erase(0, pos + 2); // 移动到 "\r\n" 后面
	}

	// 检查是否还有剩余的字符串
	if (!temp.empty()) {
		temp = removeSegmentRegisters(temp);
		str.push_back(temp);
	}
}


void MakePotatoPaste(int window) {
	size_t clipboardDataLen = 0;
	LPSTR pasteData = GetClipboardTextData(&clipboardDataLen);
	unsigned char* encode{};
	size_t size;

	SELECTIONDATA sel = { 0 };
	GuiSelectionGet(window, &sel);

	// const char* asm_str = "mov eax, 0x1234";
	std::vector<std::string> str;

	ProcessInput(pasteData, str);

	for (const std::string& s : str) {
		// Get the machine code for the assembly s
		if (GetAssemblyHex((LPSTR)s.c_str(), &encode, &size) == 0) {
			// Print the machine code to stdout
			dprintf("%s = ", s.c_str());
			for (size_t i = 0; i < size; i++) {
				dprintf("%02X ", encode[i]);
			}
			dprintf("\n");
			DbgFunctions()->MemPatch(sel.start, encode, size);
			sel.start += size;
		}
	}

	ks_free(encode);
	if (window == GUI_DISASSEMBLY) {
		GuiUpdateDisassemblyView();
	}
	else if (window == GUI_DUMP) {
		GuiUpdateDumpView();
	}


}

PLUG_EXPORT void CBINITDEBUG(CBTYPE cbType, PLUG_CB_INITDEBUG* info)
{
}

PLUG_EXPORT void CBSTOPDEBUG(CBTYPE cbType, PLUG_CB_STOPDEBUG* info)
{
}

PLUG_EXPORT void CBEXCEPTION(CBTYPE cbType, PLUG_CB_EXCEPTION* info)
{
}

PLUG_EXPORT void CBDEBUGEVENT(CBTYPE cbType, PLUG_CB_DEBUGEVENT* info)
{
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
	switch (info->hEntry)
	{

	case MENU_DISASM_PASTE:
		MakeTomatoPaste(GUI_DISASSEMBLY, FALSE);
		break;
	case MENU_DISASM_PASTE_PATCH:
		MakeTomatoPaste(GUI_DISASSEMBLY, TRUE);
		break;
	case MENU_DUMP_PASTE:
		MakeTomatoPaste(GUI_DUMP, FALSE);
		break;
	case MENU_DUMP_PASTE_PATCH:
		MakeTomatoPaste(GUI_DUMP, TRUE);
		break;
	case MENU_DISASM_ABOUT:
	case MENU_DISASM_PASTE_ASSEBLY:
		// MessageBoxA(hwndDlg, "GetAssemblyHex assembly", "Tips", MB_ICONINFORMATION);
		__try {
			MakePotatoPaste(GUI_DISASSEMBLY);
		}
		__except (1) {
			dprintf("MENU_DISASM_PASTE_ASSEBLY err");
		}
		break;
	case MENU_DUMP_PASTE_ASSEBLY:
		__try {
			MakePotatoPaste(GUI_DUMP);
		}
		__except (1) {
			dprintf("MENU_DUMP_PASTE_ASSEBLY err");
		}
	case MENU_DUMP_ABOUT:
		About();
		break;
	default:
		break;
	}
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
	if (!InitBinaryObject(0xFEED))
	{
		MessageBoxA(hwndDlg, "Ups. memory?", "yummyPaste", MB_ICONSTOP);
		return false;
	}


	return true; //Return false to cancel loading the plugin.
}


void pluginStop()
{
	DestroyBinaryObject();
}


void pluginSetup()
{
	_plugin_menuaddentry(hMenuDisasm, MENU_DISASM_PASTE, "&Paste it!");
	_plugin_menuaddentry(hMenuDisasm, MENU_DISASM_PASTE_PATCH, "Paste and Patch");
	_plugin_menuaddentry(hMenuDisasm, MENU_DISASM_PASTE_ASSEBLY, "Paste assembly");
	_plugin_menuaddentry(hMenuDisasm, MENU_DISASM_ABOUT, "A&bout");

	_plugin_menuaddentry(hMenuDump, MENU_DUMP_PASTE, "&Paste it!");
	_plugin_menuaddentry(hMenuDump, MENU_DUMP_PASTE_PATCH, "Paste and Patch");
	_plugin_menuaddentry(hMenuDump, MENU_DUMP_PASTE_ASSEBLY, "Paste assembly");
	_plugin_menuaddentry(hMenuDump, MENU_DUMP_ABOUT, "A&bout");
}
