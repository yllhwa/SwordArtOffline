<script setup>
import { NIcon } from "naive-ui";
import { Home20Regular, DataPie20Regular, Settings20Regular } from "@vicons/fluent";
import { NConfigProvider } from 'naive-ui';
import { EventsOn } from "../wailsjs/runtime";
import { Base64 } from 'js-base64';
import { getTagByData } from "./utils.js";
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
  // 参数从第4个开始
  let params = dataArr.slice(4, dataArr.length - 2);
  let paramsArr = [];
  // 参数两个一组，第一个是类型，第二个是数据，如果第一个中有(base64)则数据用base64解码
  for (let i = 0; i < params.length; i += 2) {
    let paramType = params[i];
    let paramData = params[i + 1];
    if (paramType.includes('(base64)')) {
      paramType = paramType.replace('(base64)', '');
      paramData = Base64.decode(paramData);
    }
    paramsArr.push([paramType, paramData]);
  }
  let _data = {
    pid: dataArr[1],
    funcName: dataArr[3],
    params: paramsArr,
    result: dataArr[dataArr.length - 2].includes('(base64)') ?
      Base64.decode(dataArr[dataArr.length - 1]) : dataArr[dataArr.length - 1]
  };
  _data.tag = getTagByData(_data);
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
