let folderCount = {};

let getFolderByPath = (path) => {
  let folder = path.split("\\");
  if (folder.length == 1) {
    folder = path.split("/");
  }
  folder.pop();
  if (folder.length == 0) {
    return "";
  }
  return folder.join("\\");
};

let getFilenameByPath = (path) => {
  let filename = path.split("\\");
  if (filename.length == 1) {
    filename = path.split("/");
  }
  return filename.pop();
};

let getTagByData = (data) => {
  let tag = {
    type: "正常",
    message: "",
    en: "success",
  };
  if (data.funcName == "CreateFileA") {
    // 若发生自我复制
    if (data.params[1][1].endsWith("(danger)")) {
      tag.type = "危险";
      tag.message = "自我复制";
      tag.en = "error";
    } else if (
      (data.params[0][1].endsWith(".exe") ||
        data.params[0][1].endsWith(".dll") ||
        data.params[0][1].endsWith(".ocx")) &&
      data.params[1][1] == "1073741824"
      // hex: 0x4000000
    ) {
      tag.type = "危险";
      tag.message = "写入可执行文件";
      tag.en = "error";
    } else {
      let folder = getFolderByPath(data.params[0][1]);
      if (folder != "") {
        if (folderCount[folder] == undefined) {
          folderCount[folder] = 1;
        } else {
          folderCount[folder]++;
        }
        // 若folderCount总的内容数>=3，则认为是恶意
        if (Object.keys(folderCount).length >= 3) {
          tag.type = "危险";
          tag.message = "写入多个文件夹";
          tag.en = "error";
        }
      } else {
      }
    }
  } else if (data.funcName == "HeapFree") {
    tag.type = "危险";
    tag.message = "异常内存释放";
    tag.en = "error";
  } else if (
    data.funcName == "RegCreateKeyEx" &&
    (data.params[1][1] ==
      "Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce" ||
      data.params[1][1] ==
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce" ||
      data.params[1][1] ==
        "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices" ||
      data.params[1][1] ==
        "SOFTWARE\\Microsoft\\Windows\\ CurrentVersion\\RunServices" ||
      data.params[1][1] ==
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce" ||
      data.params[1][1] ==
        "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce" ||
      data.params[1][1] ==
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run" ||
      data.params[1][1] == "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")
  ) {
    tag.type = "危险";
    tag.message = "修改注册表自启动";
    tag.en = "error";
  } else if (
    data.funcName == "HeapDestroy" &&
    data.params[0][1].endsWith("(danger)")
  ) {
    tag.type = "危险";
    tag.message = "异常内存销毁";
    tag.en = "error";
  } else if (
    data.funcName == "send" &&
    data.params[0][1].endsWith("(danger)")
  ) {
    tag.type = "危险";
    tag.message = "读取本地内容并发送";
    tag.en = "error";
  }
  return tag;
};

let getFileModeBydwDesiredAccess = (dwDesiredAccess) => {
  const GENERIC_READ = 0x80000000;
  const GENERIC_WRITE = 0x40000000;
  const GENERIC_EXECUTE = 0x20000000;
  const GENERIC_ALL = 0x10000000;
  const access = parseInt(dwDesiredAccess);
  let mode = "";
  if (access & GENERIC_READ) {
    mode += "可读";
  }
  if (access & GENERIC_WRITE) {
    mode += "可写";
  }
  if (access & GENERIC_EXECUTE) {
    mode += "可执行";
  }
  if (access & GENERIC_ALL) {
    mode += "所有权限";
  }
  return mode;
};
let fileOperationCache = {};
/*
  {
    "hFile": {
      "filename": "filename",
      "mode": "mode",
      "operation": []
    }
  }
*/
let heapOperationCache = {};
/*
  {
    "hHeap": {
      "operation": []
    }
  }
*/
let regOperationCache = {};
/*
  {
    "hKey": {
      "path": "path",
      "operation": []
    }
  }
*/
let socketOperationCache = {};
/*
  {
    "s": {
      "protocol": "protocol",
      "addr": "addr",
      "operation": []
    }
  }
*/
// 通过传入的信息获取一句简短的总结供展示
let getConclusionByMessage = (message) => {
  switch (message.funcName) {
    case "MessageBoxA":
    case "MessageBoxW":
      return `弹出对话框，标题为[${message.params[2][1]}]，内容为[${message.params[1][1]}]`;
    case "CreateFileA":
      fileOperationCache[message.result] = {
        filename: message.params[0][1],
        mode: getFileModeBydwDesiredAccess(message.params[1][1]),
        operation: [],
      };
      return `打开文件[${
        message.params[0][1]
      }]，打开模式为[${getFileModeBydwDesiredAccess(message.params[1][1])}]`;
    case "WriteFile":
      if (fileOperationCache[message.params[0][1]] != undefined) {
        fileOperationCache[message.params[0][1]].operation.push(
          `写入[${message.params[2][1]}]字节`
        );
      }
      return `写入文件[${
        fileOperationCache[message.params[0][1]]?.filename
      }]，共[${message.params[2][1]}]字节`;
    case "ReadFile":
      if (fileOperationCache[message.params[0][1]] != undefined) {
        fileOperationCache[message.params[0][1]].operation.push(`读取文件`);
      }
      return `读取文件[${fileOperationCache[message.params[0][1]]?.filename}]`;
    case "HeapCreate":
      heapOperationCache[message.result] = {
        operation: [],
      };
      return `创建堆，堆初始大小为[${message.params[1][1]}]，堆最大大小为[${message.params[2][1]}]`;
    case "HeapAlloc":
      if (heapOperationCache[message.params[0][1]] != undefined) {
        heapOperationCache[message.params[0][1]].operation.push(
          `分配[${message.params[2][1]}]字节，地址为[${message.result}]`
        );
      }
      return `在堆[${message.params[0][1]}]中分配[${message.params[2][1]}]字节`;
    case "HeapFree":
      if (heapOperationCache[message.params[0][1]] != undefined) {
        heapOperationCache[message.params[0][1]].operation.push(
          `释放[${message.params[2][1]}]字节，地址为[${message.params[1][1]}]`
        );
      }
      return `释放堆[${message.params[0][1]}]中的内存[${message.params[1][1]}]`;
    case "HeapDestroy":
      if (heapOperationCache[message.params[0][1]] != undefined) {
        heapOperationCache[message.params[0][1]].operation.push(`销毁堆`);
      }
      return `销毁堆[${message.params[0][1]}]`;
    case "RegCreateKeyEx":
      regOperationCache[message.params[7][1]] = {
        path: message.params[1][1],
        operation: [],
      };
      return `创建注册表键[${message.params[1][1]}]`;
    case "RegOpenKeyEx":
      regOperationCache[message.params[4][1]] = {
        path: message.params[1][1],
        operation: [],
      };
      return `打开注册表键[${message.params[1][1]}]`;
    case "RegSetValueEx":
      if (regOperationCache[message.params[0][1]] != undefined) {
        regOperationCache[message.params[0][1]].operation.push(
          `设置键值[${message.params[1][1]}]为[${message.params[4][1]}]`
        );
      }
      return `设置注册表键[${
        regOperationCache[message.params[0][1]]?.path
      }]的键值[${message.params[1][1]}]为[${message.params[4][1]}]`;
    case "RegCloseKey":
      if (regOperationCache[message.params[0][1]] != undefined) {
        regOperationCache[message.params[0][1]].operation.push(`关闭注册表键`);
      }
      return `关闭注册表键[${regOperationCache[message.params[0][1]]?.path}]`;
    case "RegDeleteKey":
      if (regOperationCache[message.params[0][1]] != undefined) {
        regOperationCache[message.params[0][1]].operation.push(`删除注册表键`);
      }
      return `删除注册表键[${regOperationCache[message.params[0][1]]?.path}]`;
    case "RegDeleteValue":
      if (regOperationCache[message.params[0][1]] != undefined) {
        regOperationCache[message.params[0][1]].operation.push(
          `删除键值[${message.params[1][1]}]`
        );
      }
      return `删除注册表键[${
        regOperationCache[message.params[0][1]]?.path
      }]的键值[${message.params[1][1]}]`;
    case "socket":
      const socketType = {
        1: "SOCK_STREAM",
        2: "SOCK_DGRAM",
        3: "SOCK_RAW",
        4: "SOCK_RDM",
        5: "SOCK_SEQPACKET",
        10: "SOCK_PACKET",
      };
      let protocol = socketType[message.params[1][1]];
      socketOperationCache[message.result] = {
        protocol: protocol,
        addr: "",
        operation: [],
      };
      return `创建套接字，协议为[${protocol}]`;
    case "connect":
      if (socketOperationCache[message.params[0][1]] != undefined) {
        socketOperationCache[message.params[0][1]].operation.push(
          `连接到[${message.params[1][1]}]`
        );
        socketOperationCache[message.params[0][1]].addr = message.params[1][1];
      }
      return `连接套接字[${message.params[0][1]}]到[${message.params[1][1]}]`;
    case "bind":
      if (socketOperationCache[message.params[0][1]] != undefined) {
        socketOperationCache[message.params[0][1]].operation.push(
          `绑定到[${message.params[1][1]}]`
        );
        socketOperationCache[message.params[0][1]].addr = message.params[1][1];
      }
      return `绑定套接字[${message.params[0][1]}]到[${message.params[1][1]}]`;
    case "send":
      if (socketOperationCache[message.params[0][1]] != undefined) {
        socketOperationCache[message.params[0][1]].operation.push(
          `发送[${message.params[2][1]}]字节数据到[${
            socketOperationCache[message.params[0][1]].addr
          }]`
        );
      }
      return `发送[${message.params[2][1]}]字节数据到地址[${
        socketOperationCache[message.params[0][1]]?.addr
      }]`;
    case "recv":
      if (socketOperationCache[message.params[0][1]] != undefined) {
        socketOperationCache[message.params[0][1]].operation.push(
          `接收[${message.params[2][1]}]字节数据`
        );
      }
      return `从地址[${socketOperationCache[message.params[0][1]]?.addr}]接收[${
        message.params[2][1]
      }]字节数据`;
    case "close":
      if (socketOperationCache[message.params[0][1]] != undefined) {
        socketOperationCache[message.params[0][1]].operation.push(`关闭套接字`);
      }
      return `关闭套接字[${message.params[0][1]}]`;
    default:
      return ``;
  }
};

export { getTagByData, getConclusionByMessage };
