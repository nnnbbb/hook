- ### 搜索包

```cmd
./vcpkg search keystone
```

- ### 安装

```cmd
./vcpkg install capstone[x86]:x64-windows-static
./vcpkg install capstone[x86]:x86-windows-static
./vcpkg install zydis:x64-windows
./vcpkg install zydis:x86-windows
./vcpkg install openssl-windows:x64-windows-static
./vcpkg install openssl-windows:x86-windows-static
./vcpkg install rapidjson:x64-windows-static
./vcpkg install rapidjson:x86-windows-static
```

- ### 安装动态链接版本

```cmd
./vcpkg install zydis:x64-windows
./vcpkg install zydis:x86-windows
```

- ### 安装静态链接版本

```cmd
./vcpkg install zydis:x64-windows-static
./vcpkg install zydis:x86-windows-static
```

- ##### 生成前事件 生成后事件

```
xajh-dll.dll
使用动态链接 4.19M
使用动态链接 4.59M
```

- ## vcpkg 使用静态链接

1. 找到项目中的.vcxproj 文件 找如下属性

```xml
<PropertyGroup Label="Globals">
    <VCProjectVersion>16.0</VCProjectVersion>
    <Keyword>Win32Proj</Keyword>
    <ProjectGuid>{762d10d6-36f8-495b-91ba-969be96c5959}</ProjectGuid>
    <RootNamespace>VulkanRayTracing</RootNamespace>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
</PropertyGroup>
```

2. 添加内容，并重新加载解决方案

```
<VcpkgTriplet Condition="'$(Platform)'=='Win32'">x86-windows-static</VcpkgTriplet>
<VcpkgTriplet Condition="'$(Platform)'=='x64'">x64-windows-static</VcpkgTriplet>
```

3. 更改项目属性

```
右键解决方案 => C/C++ => 代码生成 => 运行库 => 多线程调试(/Mtd)
```

【【vcpkg】visual studio 链接静态库-哔哩哔哩】 https://b23.tv/9X4uIvc

./vcpkg install keystone:x86-windows
./vcpkg install keystone:x86-windows-static
./vcpkg install keystone:x64-windows
./vcpkg install keystone:x64-windows-static

./vcpkg remove keystone:x86-windows
./vcpkg remove keystone:x86-windows-static
./vcpkg remove keystone:x64-windows
./vcpkg remove keystone:x64-windows-static

1. vcpkg 指定特定的工具集
   一台机器上同时安装了多个版本的 Visual Studio 或是一个 Visual Studio 安装了多个版本的工具集时，vcpkg 会默认调用最新的工具集，若想使用旧的工具集，可通过修改指定 triplet 文件来实现：
   vs2019
   修改 triplets/x64-windows.cmake
   set(VCPKG_PLATFORM_TOOLSET v142)

2. 添加额外的编译选项
   在具体模块对应 port 的 portfile.cmake 文件中添加如下内容来添加额外的编译选项：

set(VCPKG_CXX_FLAGS "/arch:SSE2 ${VCPKG_CXX_FLAGS}")
set(VCPKG_C_FLAGS "/arch:SSE2 ${VCPKG_C_FLAGS}")

https://www.cnblogs.com/vcpkg/p/15019968.html

### vcpkg 如何全面卸载和重新安装包

如果要 vcpkg 启动全新的安装，当用 remove 是不行的，一定要把

C:\Users\nnn\AppData\Local\vcpkg

里的内容清空掉才行。

1. yummyPaste\yummyPaste.vcxproj.filters
    <ClInclude Include="pluginsdk\keystone\arm.h">
      <Filter>Header Files\pluginsdk\keystone</Filter>
    </ClInclude>
    <ClInclude Include="pluginsdk\keystone\arm64.h">
      <Filter>Header Files\pluginsdk\keystone</Filter>
    </ClInclude>
    <ClInclude Include="pluginsdk\keystone\evm.h">
      <Filter>Header Files\pluginsdk\keystone</Filter>
    </ClInclude>
    <ClInclude Include="pluginsdk\keystone\hexagon.h">
      <Filter>Header Files\pluginsdk\keystone</Filter>
    </ClInclude>
    <ClInclude Include="pluginsdk\keystone\keystone.h">
      <Filter>Header Files\pluginsdk\keystone</Filter>
    </ClInclude>
    <ClInclude Include="pluginsdk\keystone\mips.h">
      <Filter>Header Files\pluginsdk\keystone</Filter>
    </ClInclude>
    <ClInclude Include="pluginsdk\keystone\ppc.h">
      <Filter>Header Files\pluginsdk\keystone</Filter>
    </ClInclude>
    <ClInclude Include="pluginsdk\keystone\sparc.h">
      <Filter>Header Files\pluginsdk\keystone</Filter>
    </ClInclude>
    <ClInclude Include="pluginsdk\keystone\systemz.h">
      <Filter>Header Files\pluginsdk\keystone</Filter>
    </ClInclude>
    <ClInclude Include="pluginsdk\keystone\x86.h">
      <Filter>Header Files\pluginsdk\keystone</Filter>
    </ClInclude>

   <Library Include="pluginsdk\keystone\keystone_x86.lib">
      <Filter>Library Files</Filter>
   </Library>
   <Library Include="pluginsdk\keystone\keystone_x64.lib">
      <Filter>Library Files</Filter>
   </Library>

2. yummyPaste\yummyPaste.vcxproj
   <ClInclude Include="pluginsdk\keystone\arm.h" />
   <ClInclude Include="pluginsdk\keystone\arm64.h" />
   <ClInclude Include="pluginsdk\keystone\evm.h" />
   <ClInclude Include="pluginsdk\keystone\hexagon.h" />
   <ClInclude Include="pluginsdk\keystone\keystone.h" />
   <ClInclude Include="pluginsdk\keystone\mips.h" />
   <ClInclude Include="pluginsdk\keystone\ppc.h" />
   <ClInclude Include="pluginsdk\keystone\sparc.h" />
   <ClInclude Include="pluginsdk\keystone\systemz.h" />
   <ClInclude Include="pluginsdk\keystone\x86.h" />
   <Library Include="pluginsdk\keystone\keystone_x64.lib">
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">true</ExcludedFromBuild>
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">true</ExcludedFromBuild>
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Release|x64'">false</ExcludedFromBuild>
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">false</ExcludedFromBuild>
   </Library>
   <Library Include="pluginsdk\keystone\keystone_x86.lib">
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">false</ExcludedFromBuild>
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">false</ExcludedFromBuild>
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Release|x64'">true</ExcludedFromBuild>
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">true</ExcludedFromBuild>
   </Library>

3. yummyPaste\pluginmain.h
   #include "pluginsdk/keystone/keystone.h"

#ifdef \_WIN64
#pragma comment(lib, "pluginsdk/keystone/keystone_x64.lib")
#else
#pragma comment(lib, "pluginsdk/keystone/keystone_x86.lib")
#endif //\_WIN64

#### 自定义输出目录

<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
  <TargetExt>.dp32</TargetExt>
  <OutDir>$(SolutionDir)bin\x32\</OutDir>
</PropertyGroup>
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
  <TargetExt>.dp32</TargetExt>
  <OutDir>$(SolutionDir)bin\x32d\</OutDir>
</PropertyGroup>
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
  <TargetExt>.dp64</TargetExt>
  <OutDir>$(SolutionDir)bin\x64\</OutDir>
</PropertyGroup>
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
  <TargetExt>.dp64</TargetExt>
  <OutDir>$(SolutionDir)bin\x64d\</OutDir>
</PropertyGroup>
