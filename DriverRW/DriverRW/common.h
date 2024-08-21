#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <ntimage.h>
#include <ntstrsafe.h>
#include "utils.hpp"

typedef unsigned long u32;
typedef unsigned long long u64;
typedef signed long long i64;
typedef volatile unsigned long long vu64;
typedef unsigned int ui32;
typedef void* pv;
typedef void* pv64;
typedef unsigned short u16;
typedef unsigned char u8;
typedef volatile unsigned char vu8;
#define __db  __debugbreak
#define noinl __declspec(noinline)
#define naked __declspec(naked)
#define inl   __forceinline
