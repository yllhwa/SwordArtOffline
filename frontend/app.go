package main

import (
	"bytes"
	"context"
	"fmt"
	"io/ioutil"
	"net"
	"os"
	"strings"
	"syscall"

	// "os"
	"os/exec"

	"github.com/wailsapp/wails/v2/pkg/runtime"
	"golang.org/x/text/encoding/simplifiedchinese"
	"golang.org/x/text/transform"
)

// App struct
type App struct {
	ctx context.Context
}

// NewApp creates a new App application struct
func NewApp() *App {
	return &App{}
}

func (app *App) startup(ctx context.Context) {
	app.ctx = ctx
	// 最大化窗口
	runtime.WindowMaximise(ctx)
	app.StartListenUDP()
}

func (app *App) StartListenUDP() {
	go func() {
		udp_addr, err := net.ResolveUDPAddr("udp", "127.0.0.1:6000")
		if err != nil {
			fmt.Println(err)
			return
		}
		udp_conn, err := net.ListenUDP("udp", udp_addr)
		if err != nil {
			fmt.Println(err)
			return
		}

		buf := make([]byte, 655350)
		for {
			n, _, err := udp_conn.ReadFromUDP(buf)
			if err != nil {
				fmt.Println(err)
				return
			}
			// fmt.Println(string(buf[:n]))
			runtime.EventsEmit(app.ctx, "udpMessage", string(buf[0:n]))
		}
	}()
}

func (app *App) InjectExe(targetPath string) {
	// 开启新进程: inject.exe targetPath
	cmd := exec.Command("Injector.exe", targetPath)
	cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}
	err := cmd.Start()
	if err != nil {
		println("启动失败:", err)
	} else {
		println("启动成功!")
	}
}

// 打开文件选择框
func (app *App) OpenExeDialog() string {
	file, _ := runtime.OpenFileDialog(app.ctx, runtime.OpenDialogOptions{
		Title: "选择文件",
		Filters: []runtime.FileFilter{
			{
				DisplayName: "*.exe",
				Pattern:     "*.exe",
			},
		},
	})
	return file
}

type FileInfo struct {
	Name    string `json:"name"`
	Path    string `json:"path"`
	Size    int64  `json:"size"`
	ModTime string `json:"modTime"`
}

// GbkToUtf8 gbk转utf8
func GbkToUtf8(src []byte) ([]byte, error) {
	reader := transform.NewReader(bytes.NewReader(src), simplifiedchinese.GBK.NewDecoder())
	d, e := ioutil.ReadAll(reader)
	if e != nil {
		return nil, e
	}
	return d, nil
}

func (app *App) GetFileInfoByPid(pid int) FileInfo {
	// 通过程序pid获取文件信息
	// 通过wmic获取进程信息
	cmd := exec.Command("wmic", "process", "where", "processid="+fmt.Sprint(pid), "get", "executablepath")
	cmd.SysProcAttr = &syscall.SysProcAttr{HideWindow: true}
	out, err := cmd.Output()
	// gbk转utf8
	out, err = GbkToUtf8(out)
	if err != nil {
		println("获取文件信息失败:", err)
		return FileInfo{}
	}
	lines := strings.Split(string(out), "\n")
	if len(lines) < 2 {
		println("获取文件信息失败:", err)
		return FileInfo{}
	}
	filePath := strings.TrimSpace(lines[1])
	fmt.Println(filePath)
	fileInfo, err := os.Stat(filePath)
	if err != nil {
		println("获取文件信息失败:", err)
		return FileInfo{}
	}

	// 时间格式化为字符串
	jsFileInfo := FileInfo{
		Name:    fileInfo.Name(),
		Path:    filePath,
		Size:    fileInfo.Size(),
		ModTime: fileInfo.ModTime().Format("2006-01-02 15:04:05"),
	}
	return jsFileInfo
}

func (app *App) ReadFile(filePath string) string {
	file, err := os.Open(filePath)
	if err != nil {
		println("读取文件失败:", err)
		return ""
	}
	defer file.Close()
	// 读取文件内容
	data, err := ioutil.ReadAll(file)
	if err != nil {
		println("读取文件失败:", err)
		return ""
	}
	return string(data)
}

func (app *App) OpenFileByDefaultApplication(filePath string) {
	err := exec.Command("cmd", "/c", "start", filePath).Start()
	if err != nil {
		println("打开文件失败:", err)
	}
}
