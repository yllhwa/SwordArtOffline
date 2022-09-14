
// MFCApplication2Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"

#include <Windows.h>
#include <string>
#include "afxdialogex.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication2Dlg 对话框



CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication2Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication2Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCApplication2Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCApplication2Dlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFCApplication2Dlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CMFCApplication2Dlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CMFCApplication2Dlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CMFCApplication2Dlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CMFCApplication2Dlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CMFCApplication2Dlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CMFCApplication2Dlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CMFCApplication2Dlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CMFCApplication2Dlg::OnBnClickedButton13)
END_MESSAGE_MAP()


// CMFCApplication2Dlg 消息处理程序

BOOL CMFCApplication2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCApplication2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication2Dlg::OnBnClickedButton1()
{
	HWND hwnd = ::FindWindow(NULL, _T("MFCApplication2"));
	MessageBoxA(hwnd, "测试MessageBoxA", "测试", MB_OK);
}


void CMFCApplication2Dlg::OnBnClickedButton2()
{
	MessageBoxW(L"测试MessageBoxW", L"测试", MB_OK);
}



void CMFCApplication2Dlg::OnBnClickedButton3()
{
	HANDLE handle = CreateFileA("test.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(handle, "测试WriteFile", strlen("测试WriteFile"), NULL, NULL);
	CloseHandle(handle);
	// 重新打开
	char buffer[1024] = { 0 };
	handle = CreateFileA("test.exe", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ReadFile(handle, buffer, sizeof(buffer), NULL, NULL);
	CloseHandle(handle);
}


void CMFCApplication2Dlg::OnBnClickedButton4()
{
	HANDLE handle = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0);
	LPVOID lpMem = HeapAlloc(handle, 0, 1024);
	HeapFree(handle, 0, lpMem);
	// HeapFree(handle, 0, lpMem);
	HeapDestroy(handle);
}


void CMFCApplication2Dlg::OnBnClickedButton5()
{
	HKEY hKey = NULL;
	TCHAR* subKey = _T("SOFTWARE\\Test");
	// 以非管理员权限创建注册表
	RegCreateKeyEx(HKEY_CURRENT_USER, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	// 写入注册表
	RegSetValueEx(hKey, _T("Test"), 0, REG_SZ, (LPBYTE)_T("TestRegSetValueEx"), sizeof(wchar_t)*strlen("TestRegSetValueEx"));
	// 读取注册表
	TCHAR buffer[1024];
	DWORD dwSize = sizeof(buffer);
	RegQueryValueEx(hKey, _T("Test"), 0, NULL, (BYTE*)buffer, &dwSize);
	//MessageBoxW( buffer, _T("测试RegQueryValueEx"), MB_OK);
	// 删除注册表
	RegDeleteValue(hKey, _T("Test"));
	RegCloseKey(hKey);
	
}

void CMFCApplication2Dlg::OnBnClickedButton6()
{
	WSADATA wsaData;
	SOCKET Socket;
	SOCKADDR_IN SockAddr;
	int lineCount = 0;
	int rowCount = 0;
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	char buffer[10000];
	int i = 0;
	int nDataLength;
	std::string website_HTML;

	// website url
	std::string url = "cse.hust.edu.cn";

	//HTTP GET
	std::string get_http = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
	WSAStartup(MAKEWORD(1, 1), &wsaData);

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	getaddrinfo(url.c_str(), "80", &hints, &result);

	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr.s_addr;

	connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr));
	// send GET / HTTP
	send(Socket, get_http.c_str(), strlen(get_http.c_str()), 0);

	// recieve html
	while ((nDataLength = recv(Socket, buffer, 10000, 0)) > 0) {
		int i = 0;
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {

			website_HTML += buffer[i];
			i += 1;
		}
	}
	closesocket(Socket);
	WSACleanup();
}

#define nil (0)
#define DESTINATION_IP "127.0.0.1"
#define DESTINATION_PORT 7000

SOCKET sockClient;
SOCKADDR_IN addrSrv;
void sendUdpPacked(const char* buffer) {
	//send(sockClient, buffer, strlen(buffer), 0);
	sendto(sockClient, buffer, (int)strlen(buffer), 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
}

int initUDPClient() {
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsadata;
	int err;
	err = WSAStartup(wVersionRequested, &wsadata);
	if (err != nil) {
		return -1;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) {
		WSACleanup();
		return -1;
	}
	sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	inet_pton(AF_INET, DESTINATION_IP, &addrSrv.sin_addr);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(DESTINATION_PORT);
	return 0;
}

void cleanUDPClient() {
	closesocket(sockClient);
	WSACleanup();
}

void CMFCApplication2Dlg::OnBnClickedButton7()
{
	initUDPClient();
	sendUdpPacked("Hello World");
	cleanUDPClient();
}


void CMFCApplication2Dlg::OnBnClickedButton8()
{
	HANDLE handle = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 1024, 1024);
	try {
		HeapFree(handle, 0, 0);
	}
	catch(...) {
	}
	HeapDestroy(handle);
}


void CMFCApplication2Dlg::OnBnClickedButton9()
{
	HKEY hKey = NULL;
	TCHAR* subKey = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	// 以非管理员权限创建注册表
	RegCreateKeyEx(HKEY_CURRENT_USER, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	// 写入注册表
	RegSetValueEx(hKey, _T("Test"), 0, REG_SZ, (LPBYTE)_T("TestRegSetValueEx"), sizeof(wchar_t) * strlen("TestRegSetValueEx"));
	// 读取注册表
	TCHAR buffer[1024];
	DWORD dwSize = sizeof(buffer);
	RegQueryValueEx(hKey, _T("Test"), 0, NULL, (BYTE*)buffer, &dwSize);
	//MessageBoxW( buffer, _T("测试RegQueryValueEx"), MB_OK);
	// 删除注册表
	RegDeleteValue(hKey, _T("Test"));
	RegCloseKey(hKey);
}


void CMFCApplication2Dlg::OnBnClickedButton10()
{
	HANDLE handle = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0);
	LPVOID lpMem = HeapAlloc(handle, 0, 1024);
	HeapFree(handle, 0, lpMem);
	HeapDestroy(handle);
	HeapDestroy(0);
}


void CMFCApplication2Dlg::OnBnClickedButton11()
{
	// 创建testFolder文件夹
	CreateDirectory(_T("testFolder"), NULL);
	CreateDirectory(_T("testFolder\\1"), NULL);
	CreateDirectory(_T("testFolder\\2"), NULL);
	CreateDirectory(_T("testFolder\\3"), NULL);
	
	HANDLE handle = CreateFileA("testFolder/1/test.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(handle, "测试WriteFile", strlen("测试WriteFile"), NULL, NULL);
	CloseHandle(handle);
	handle = CreateFileA("testFolder/2/test.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(handle, "测试WriteFile", strlen("测试WriteFile"), NULL, NULL);
	CloseHandle(handle);
	handle = CreateFileA("testFolder/3/test.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(handle, "测试WriteFile", strlen("测试WriteFile"), NULL, NULL);
	CloseHandle(handle);
}

std::string getFilenameByPath(std::string path) {
	int pos = path.find_last_of("\\");
	if (pos == -1) {
		pos = path.find_last_of("/");
	}
	return path.substr(pos + 1);
}

void CMFCApplication2Dlg::OnBnClickedButton12()
{
	// 自我复制
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	// 转为LPCSTR
	char* szFileNameA = new char[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, szFileName, -1, szFileNameA, MAX_PATH, NULL, NULL);
	// 读取自身后写入
	HANDLE hFile = CreateFileA(szFileNameA, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	LPVOID lpFileBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize);
	ReadFile(hFile, lpFileBuffer, dwFileSize, NULL, NULL);
	CloseHandle(hFile);
	// 写入testCopy文件夹
	CreateDirectory(_T("testCopy"), NULL);
	// testCopy+自身名称
	std::string filename = getFilenameByPath(szFileNameA);
	std::string path = "testCopy/" + filename;
	hFile = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, lpFileBuffer, dwFileSize, NULL, NULL);
	CloseHandle(hFile);
}


void CMFCApplication2Dlg::OnBnClickedButton13()
{
	// 读取文件并发送
	// C:\Windows\System32\drivers\etc\hosts
	HANDLE hFile = CreateFileA("C:\\Windows\\System32\\drivers\\etc\\hosts", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	LPVOID lpFileBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize+1);
	memset(lpFileBuffer, 0, dwFileSize + 1);
	ReadFile(hFile, lpFileBuffer, dwFileSize, NULL, NULL);
	CloseHandle(hFile);
	// 发送udp包
	initUDPClient();
	sendUdpPacked((char*)lpFileBuffer);
	cleanUDPClient();
}
