<script setup>
import { NIcon } from "naive-ui";
import { Home20Regular, DataPie20Regular, Settings20Regular } from "@vicons/fluent";
import { NConfigProvider } from 'naive-ui';
import { EventsOn } from "../wailsjs/runtime";
import { Base64 } from 'js-base64';
import { setTagByData, setConclusionByMessage } from "./utils.js";
import { store } from "./store.js";
const themeOverrides = {
  common: {
    primaryColor: '#0078d7',
    primaryColorHover: '#0078d7',
    primaryColorPressed: '#0078d7',
    primaryColorSuppl: '#0078d7',
  }
}


EventsOn('udpMessage', (data) => {
  // console.log(data);
  // data用\n分割
  let dataArr = data.split('\n');
  let _data = {
    params: []
  };
  for (let i = 0; i < dataArr.length; i += 2) {
    // 若有(base64)
    if (dataArr[i].includes('(base64)')) {
      dataArr[i] = dataArr[i].replace('(base64)', '');
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
          value: dataArr[i + 1]
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
});
</script>

<template>
  <n-config-provider :theme-overrides="themeOverrides" abstract>
    <div class="flex flex-row h-full">
      <div class="flex flex-col w-1/5 py-2 bg-gray-100 select-none shadow-xl">
        <router-link to="/index" class="py-2 hover:bg-gray-200" v-slot="{ isActive }">
          <div class="menu-item" :class="{ 'item-selected': isActive }">
            <n-icon size="1.5em" :component="Home20Regular" />
            <span class="ml-3">主页</span>
          </div>
        </router-link>
        <router-link to="/analysis" class="py-2 hover:bg-gray-200" v-slot="{ isActive }">
          <div class="menu-item" :class="{ 'item-selected': isActive }">
            <n-icon size="1.5em" :component="DataPie20Regular" />
            <span class="ml-3">行为分析</span>
          </div>
        </router-link>
        <router-link to="/setting" class="py-2 hover:bg-gray-200" v-slot="{ isActive }">
          <div class="menu-item" :class="{ 'item-selected': isActive }">
            <n-icon size="1.5em" :component="Settings20Regular" />
            <span class="ml-3">设置</span>
          </div>
        </router-link>
      </div>
      <router-view class="w-4/5"></router-view>
    </div>
  </n-config-provider>
</template>

<style scoped>
.menu-item {
  @apply px-2 flex flex-row items-center box-content border-l-4 border-l-blue-600 border-opacity-0;
}

.item-selected {
  @apply border-opacity-100;
}
</style>
