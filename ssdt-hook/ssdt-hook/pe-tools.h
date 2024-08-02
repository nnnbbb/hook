#pragma once
#include<ntifs.h>

NTSTATUS PeInitialize();

void PeDeinitialize();

int GetExportSsdtIndex(const char* ExportName);
