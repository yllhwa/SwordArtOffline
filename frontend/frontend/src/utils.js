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
  console.log(data);
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
    }
    else if (
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
  } else if(
    data.funcName=="send" && data.params[0][1].endsWith("(danger)")
  )
  {
    tag.type = "危险";
    tag.message = "读取本地内容并发送";
    tag.en = "error";
  }
  return tag;
};

export { getTagByData };
