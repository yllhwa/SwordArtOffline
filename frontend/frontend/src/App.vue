<script setup>
import { NIcon } from "naive-ui";
import { NConfigProvider } from 'naive-ui';

import hljs from 'highlight.js/lib/core'
import lua from 'highlight.js/lib/languages/lua'

import { EventsOn } from "../wailsjs/runtime";

import { dealUdpMessage } from "./utils.js";
import { routes } from "./router.js";

hljs.registerLanguage('lua', lua)

const themeOverrides = {
  common: {
    primaryColor: '#0078d7',
    primaryColorHover: '#0078d7',
    primaryColorPressed: '#0078d7',
    primaryColorSuppl: '#0078d7',
  }
}

EventsOn('udpMessage', (data) => {
  dealUdpMessage(data);
});
</script>

<template>
  <n-config-provider :theme-overrides="themeOverrides" :hljs="hljs" abstract>
    <div class="flex flex-row h-full">
      <div class="flex flex-col w-1/5 py-2 bg-gray-100 select-none shadow-xl">
        <router-link v-for="route in routes" :key="route.path" :to="route.path" class="menu-item-container"
          v-slot="{ isActive }">
          <div class="menu-item" :class="{ 'item-selected': isActive }">
            <n-icon size="1.5em" :component="route.icon" />
            <span class="ml-3">{{ route.meta?.title }}</span>
          </div>
        </router-link>
      </div>
      <router-view v-slot="{ Component }" class="w-4/5">
        <keep-alive>
          <component :is="Component" />
        </keep-alive>
      </router-view>
    </div>
  </n-config-provider>
</template>

<style scoped>
.menu-item-container {
  @apply py-2;
}

.menu-item-container:hover {
  @apply bg-gray-200;
}

.menu-item {
  @apply px-2 flex flex-row items-center box-content border-l-4 border-l-blue-600 border-opacity-0;
}

.item-selected {
  @apply border-opacity-100;
}
</style>
