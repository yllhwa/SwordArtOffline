let getTagByData = (data) => {
  let tag = {
    type: "正常",
    message: "",
    en: "success",
  };
  if (
    data.funcName == "CreateFileA" &&
    (data.params[0][1].endsWith(".exe") ||
      data.params[0][1].endsWith(".dll") ||
      data.params[0][1].endsWith(".ocx")) &&
    data.params[1][1] == "1073741824"
    // hex: 0x4000000
  ) {
    tag.type = "危险";
    tag.message = "写入可执行文件";
    tag.en = "error";
  } else if (data.funcName == "HeapFree") {
    tag.type = "危险";
    tag.message = "异常内存释放";
    tag.en = "error";
  } else if (
    data.funcName == "RegCreateKeyEx" &&
    (data.params[1][1] ==
      "Software\\Microsoft\\Windows\\ CurrentVersion\\RunServicesOnce" ||
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
  }
  return tag;
};

export { getTagByData };