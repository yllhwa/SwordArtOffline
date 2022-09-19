## 华中科技大学 网络空间安全学院 2020级 软件安全 课程设计

![CI status](https://github.com/yllhwa/SwordArtOffline/actions/workflows/build.yml/badge.svg)

### 目录说明
```
.
├── HookLib     Detours Hook程序和注射器
├── demos       测试样本
├── docs        文档等
└── frontend    wails GUI程序
```

### 编译

#### 1. HookLib
编译环境：cmake、VS2022
```powershell
mkdir build
cd build
cmake -DCI_BUILD="true" -DCMAKE_GENERATOR_PLATFORM=Win32 -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```
或直接用Clion打开编译即可

#### 2. GUI
编译环境：golang>=1.18、npm、nodejs、upx(可选)
```powershell
go install github.com/wailsapp/wails/v2/cmd/wails@latest
cd frontend
npm install
npm run build
cd ../
choco install upx
wails build -clean -upx -webview2 error
```

### 二进制文件

已开启自动构建，在Actions中可以下载

![image](./docs/imgs/workflow.png)

### LICENSE

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

请务必注意协议的限制