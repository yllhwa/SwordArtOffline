import { Base64 } from 'js-base64';
import { store } from "./store.js";

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

let setTagByData = (data) => {
  let tag = {
    type: "正常",
    message: "",
    en: "success",
  };
  switch (data.funcName) {
    case "CreateFileA":
      let folder = getFolderByPath(data.params[0].value);
      if (folder != "") {
        if (folderCount[folder] == undefined) {
          folderCount[folder] = 1;
        } else {
          folderCount[folder]++;
        }
      }
      if (data.extra == "SelfCopy") {
        tag = {
          type: "危险",
          message: "自我复制",
          en: "error",
        };
      } else if (
        [".exe", ".dll", ".ocx"].some((suffix) =>
          data.params[0].value.endsWith(suffix)
        ) &&
        data.params[1].value == "1073741824" // hex: 0x4000000
      ) {
        tag = {
          type: "危险",
          message: "写入可执行文件",
          en: "error",
        };
      } else {
        if (Object.keys(folderCount).length >= 3) {
          tag = {
            type: "危险",
            message: "写入多个文件夹",
            en: "error",
          };
        }
      }
      break;
    case "HeapFree":
      // 只有危险才会发送HeapFree消息
      tag = {
        type: "危险",
        message: "异常内存释放",
        en: "error",
      };
      break;
    case "RegCreateKeyEx":
      const startUpRegs = [
        "Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce",
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce",
        "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices",
        "SOFTWARE\\Microsoft\\Windows\\ CurrentVersion\\RunServices",
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
        "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
      ];
      if (startUpRegs.some((reg) => data.params[1].value == reg)) {
        tag = {
          type: "危险",
          message: "修改注册表自启动",
          en: "error",
        };
      }
      break;
    case "HeapDestroy":
      if (data.extra == "UntrackedHeapHandle") {
        tag = {
          type: "危险",
          message: "异常内存销毁",
          en: "error",
        };
      }
      break;
    case "send":
      if (data.extra == "ReadFileAndSend") {
        tag = {
          type: "危险",
          message: "读取本地文件并发送",
          en: "error",
        };
      }
      break;
    default:
      break;
  }
  data.tag = tag;
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
let setConclusionByMessage = (message) => {
  switch (message.funcName) {
    case "MessageBoxA":
    case "MessageBoxW":
      message.conclusion = `弹出对话框，标题为[${message.params[2].value}]，内容为[${message.params[1].value}]`;
      break;
    case "CreateFileA":
      fileOperationCache[message.result] = {
        filename: message.params[0].value,
        mode: getFileModeBydwDesiredAccess(message.params[1].value),
        operation: [],
      };
      message.conclusion = `打开文件[${
        message.params[0].value
      }]，打开模式为[${getFileModeBydwDesiredAccess(message.params[1].value)}]`;
      break;
    case "WriteFile":
      if (fileOperationCache[message.params[0].value] != undefined) {
        fileOperationCache[message.params[0].value].operation.push(
          `写入[${message.params[2].value}]字节`
        );
      }
      message.conclusion = `写入文件[${
        fileOperationCache[message.params[0].value]?.filename
      }]，共[${message.params[2].value}]字节`;
      break;
    case "ReadFile":
      if (fileOperationCache[message.params[0].value] != undefined) {
        fileOperationCache[message.params[0].value].operation.push(`读取文件`);
      }
      message.conclusion = `读取文件[${
        fileOperationCache[message.params[0].value]?.filename
      }]`;
      break;
    case "HeapCreate":
      heapOperationCache[message.result] = {
        operation: [],
      };
      message.conclusion = `创建堆，堆初始大小为[${message.params[1].value}]，堆最大大小为[${message.params[2].value}]`;
      break;
    case "HeapAlloc":
      if (heapOperationCache[message.params[0].value] != undefined) {
        heapOperationCache[message.params[0].value].operation.push(
          `分配[${message.params[2].value}]字节，地址为[${message.result}]`
        );
      }
      message.conclusion = `在堆[${message.params[0].value}]中分配[${message.params[2].value}]字节`;
      break;
    case "HeapFree":
      if (heapOperationCache[message.params[0].value] != undefined) {
        heapOperationCache[message.params[0].value].operation.push(
          `释放[${message.params[2].value}]字节，地址为[${message.params[1].value}]`
        );
      }
      message.conclusion = `释放堆[${message.params[0].value}]中的内存[${message.params[1].value}]`;
      break;
    case "HeapDestroy":
      if (heapOperationCache[message.params[0].value] != undefined) {
        heapOperationCache[message.params[0].value].operation.push(`销毁堆`);
      }
      message.conclusion = `销毁堆[${message.params[0].value}]`;
      break;
    case "RegCreateKeyEx":
      regOperationCache[message.params[7].value] = {
        path: message.params[1].value,
        operation: [],
      };
      message.conclusion = `创建注册表键[${message.params[1].value}]`;
      break;
    case "RegOpenKeyEx":
      regOperationCache[message.params[4].value] = {
        path: message.params[1].value,
        operation: [],
      };
      message.conclusion = `打开注册表键[${message.params[1].value}]`;
      break;
    case "RegSetValueEx":
      if (regOperationCache[message.params[0].value] != undefined) {
        regOperationCache[message.params[0].value].operation.push(
          `设置键值[${message.params[1].value}]为[${message.params[4].value}]`
        );
      }
      message.conclusion = `设置注册表键[${
        regOperationCache[message.params[0].value]?.path
      }]的键值[${message.params[1].value}]为[${message.params[4].value}]`;
      break;
    case "RegCloseKey":
      if (regOperationCache[message.params[0].value] != undefined) {
        regOperationCache[message.params[0].value].operation.push(
          `关闭注册表键`
        );
      }
      message.conclusion = `关闭注册表键[${
        regOperationCache[message.params[0].value]?.path
      }]`;
      break;
    case "RegDeleteKey":
      if (regOperationCache[message.params[0].value] != undefined) {
        regOperationCache[message.params[0].value].operation.push(
          `删除注册表键`
        );
      }
      message.conclusion = `删除注册表键[${
        regOperationCache[message.params[0].value]?.path
      }]`;
      break;
    case "RegDeleteValue":
      if (regOperationCache[message.params[0].value] != undefined) {
        regOperationCache[message.params[0].value].operation.push(
          `删除键值[${message.params[1].value}]`
        );
      }
      message.conclusion = `删除注册表键[${
        regOperationCache[message.params[0].value]?.path
      }]的键值[${message.params[1].value}]`;
      break;
    case "socket":
      const socketType = {
        1: "SOCK_STREAM",
        2: "SOCK_DGRAM",
        3: "SOCK_RAW",
        4: "SOCK_RDM",
        5: "SOCK_SEQPACKET",
        10: "SOCK_PACKET",
      };
      let protocol = socketType[message.params[1].value];
      socketOperationCache[message.result] = {
        protocol: protocol,
        addr: "",
        operation: [],
      };
      message.conclusion = `创建套接字，协议为[${protocol}]`;
      break;
    case "connect":
      if (socketOperationCache[message.params[0].value] != undefined) {
        socketOperationCache[message.params[0].value].operation.push(
          `连接到[${message.params[1].value}]`
        );
        socketOperationCache[message.params[0].value].addr =
          message.params[1].value;
      }
      message.conclusion = `连接套接字[${message.params[0].value}]到[${message.params[1].value}]`;
      break;
    case "bind":
      if (socketOperationCache[message.params[0].value] != undefined) {
        socketOperationCache[message.params[0].value].operation.push(
          `绑定到[${message.params[1].value}]`
        );
        socketOperationCache[message.params[0].value].addr =
          message.params[1].value;
      }
      message.conclusion = `绑定套接字[${message.params[0].value}]到[${message.params[1].value}]`;
      break;
    case "send":
      if (socketOperationCache[message.params[0].value] != undefined) {
        socketOperationCache[message.params[0].value].operation.push(
          `发送[${message.params[2].value}]字节数据到[${
            socketOperationCache[message.params[0].value].addr
          }]`
        );
      }
      message.conclusion = `发送[${message.params[2].value}]字节数据到地址[${
        socketOperationCache[message.params[0].value]?.addr
      }]`;
      break;
    case "recv":
      if (socketOperationCache[message.params[0].value] != undefined) {
        socketOperationCache[message.params[0].value].operation.push(
          `接收[${message.params[2].value}]字节数据`
        );
      }
      message.conclusion = `从地址[${
        socketOperationCache[message.params[0].value]?.addr
      }]接收[${message.params[2].value}]字节数据`;
      break;
    case "close":
      if (socketOperationCache[message.params[0].value] != undefined) {
        socketOperationCache[message.params[0].value].operation.push(
          `关闭套接字`
        );
      }
      message.conclusion = `关闭套接字[${message.params[0].value}]`;
      break;
    default:
      message.conclusion = ``;
  }
};

let getFileOperationCacheByHandle = (handle) => {
  return fileOperationCache[handle];
};

let getRegOperationCacheByHandle = (handle) => {
  return regOperationCache[handle];
};

let getSocketOperationCacheByHandle = (handle) => {
  return socketOperationCache[handle];
};

let shouldShowTrack = (message) => {
  // 文件操作、注册表操作、套接字操作
  const funcNames = [
    "CreateFileA",
    "WriteFile",
    "ReadFile",
    "CloseHandle",
    "RegCreateKeyEx",
    "RegOpenKeyEx",
    "RegSetValueEx",
    "RegCloseKey",
    "RegDeleteKey",
    "RegDeleteValue",
    "socket",
    "bind",
    "connect",
    "send",
    "recv",
    "close",
  ];
  return funcNames.includes(message.funcName);
};

const funcTypeMap = {
  弹窗: ["MessageBoxA", "MessageBoxW"],
  文件操作: ["CreateFileA", "WriteFile", "ReadFile", "CloseHandle"],
  堆操作: ["HeapCreate", "HeapAlloc", "HeapFree", "HeapDestroy"],
  注册表操作: [
    "RegCreateKeyEx",
    "RegOpenKeyEx",
    "RegSetValueEx",
    "RegCloseKey",
    "RegDeleteKey",
    "RegDeleteValue",
  ],
  网络操作: ["socket", "bind", "connect", "send", "recv", "close"],
};
let funTypeReverseMap = {};
for (let key in funcTypeMap) {
  funcTypeMap[key].forEach((item) => {
    funTypeReverseMap[item] = key;
  });
}

let getOperationCacheByFunc = (func) => {
  switch (func.funcName) {
    case "CreateFileA":
      return getFileOperationCacheByHandle(func.result);
    case "WriteFile":
    case "ReadFile":
    case "CloseHandle":
      return getFileOperationCacheByHandle(func.params[0].value);
    case "RegCreateKeyEx":
      return getRegOperationCacheByHandle(func.params[7].value);
    case "RegOpenKeyEx":
    case "RegSetValueEx":
    case "RegCloseKey":
    case "RegDeleteKey":
    case "RegDeleteValue":
      return getRegOperationCacheByHandle(func.params[0].value);
    case "socket":
      return getSocketOperationCacheByHandle(func.result);
    case "bind":
    case "connect":
    case "send":
    case "recv":
    case "close":
      return getSocketOperationCacheByHandle(func.params[0].value);
  }
};

let dealUdpMessage = (data) => {
  // console.log(data);
  // data用\n分割
  let dataArr = data.split("\n");
  let _data = {
    params: [],
  };
  for (let i = 0; i < dataArr.length; i += 2) {
    // 若有(base64)
    if (dataArr[i].includes("(base64)")) {
      dataArr[i] = dataArr[i].replace("(base64)", "");
      dataArr[i + 1] = Base64.decode(dataArr[i + 1]);
    }
    switch (dataArr[i]) {
      case "pid":
        _data.pid = dataArr[i + 1];
        break;
      case "funcName":
        _data.funcName = dataArr[i + 1];
        break;
      case "result":
        _data.result = dataArr[i + 1];
        break;
      case "extra":
        _data.extra = dataArr[i + 1];
        break;
      default:
        _data.params.push({
          name: dataArr[i],
          value: dataArr[i + 1],
        });
        break;
    }
  }
  // 若为memcpy则单独处理
  if (_data.funcName == "memcpy") {
    store.memCacheData.push(_data);
    return;
  }
  setTagByData(_data);
  setConclusionByMessage(_data);
  _data.id = store.analysisData.length;
  store.analysisData.push(_data);
};

export {
  setTagByData,
  setConclusionByMessage,
  shouldShowTrack,
  funcTypeMap,
  funTypeReverseMap,
  getOperationCacheByFunc,
  dealUdpMessage,
};
