<script setup>
import { NCard, NCheckboxGroup, NCheckbox, NSpace, NList, NListItem, NThing, NTag, NIcon, NSlider, NModal } from 'naive-ui';
import { NTabs, NTabPane } from 'naive-ui';
import { DataPie20Regular, Dismiss20Regular } from '@vicons/fluent';
import { store } from '../store.js';
import { GetFileInfoByPid } from '../../wailsjs/go/main/App'

let dataFilter = $ref(["弹窗", "文件操作", "堆操作", "注册表操作", "网络操作"]);
let dataFilterList = $ref(["弹窗", "文件操作", "堆操作", "注册表操作", "网络操作"]);
let isCheckAll = $ref(true);
let onChooseAllChecked = () => {
    dataFilter = isCheckAll ? [] : dataFilterList;
};

let funcTypeMap = {
    "弹窗": ["MessageBoxA", "MessageBoxW"],
    "文件操作": ["CreateFileA", "WriteFile", "ReadFile", "CloseHandle"],
    "堆操作": ["HeapCreate", "HeapAlloc", "HeapFree", "HeapDestroy"],
    "注册表操作": ["RegCreateKeyEx", "RegOpenKeyEx", "RegSetValueEx", "RegCloseKey", "RegDeleteKey", "RegDeleteValue"],
    "网络操作": ["socket", "bind", "connect", "send", "recv", "close"],
};

let funTypeReverseMap = {};
for (let key in funcTypeMap) {
    funcTypeMap[key].forEach(item => {
        funTypeReverseMap[item] = key;
    });
}

let levelFilter = $ref(100);

let marks = {
    0: "危险",
    50: "可疑",
    100: "全部",
}

let shouldShow = (message) => {
    let level = 0;
    if (message?.tag?.type == "危险") {
        level = 0;
    } else if (message?.tag?.type == "可疑") {
        level = 50;
    } else {
        level = 100;
    }
    if (level > levelFilter) {
        return false;
    }
    return dataFilter.includes(funTypeReverseMap[message.funcName]);
}

let showModal = $ref(false);

let fileInfo = $ref({
    Name: "",
    Path: "",
    Size: 0,
    ModTime: "",
});

let displayDetail = (message) => {
    console.log(message);
    showModal = true;
    let pid = parseInt(message.pid);
    GetFileInfoByPid(pid).then((res) => {
        fileInfo = res;
        console.log(res);
    });
}

</script>

<template>
    <div class="px-6 pt-2 pb-4 flex flex-col">
        <div class="text-2xl py-4 flex flex-row items-center">
            <n-icon size="1.25em" :component="DataPie20Regular" />
            <span class="px-2">行为分析</span>
        </div>
        <n-card title="筛选" class="my-4">
            <n-checkbox @update:checked="onChooseAllChecked" v-model:checked="isCheckAll" label="全选" />
            <n-checkbox-group v-model:value="dataFilter">
                <n-space item-style="display: flex;">
                    <n-checkbox v-for="datafilter in dataFilterList" :key="datafilter" :value="datafilter"
                        :label="datafilter" />
                </n-space>
            </n-checkbox-group>
            <div class="mt-2 w-1/2">
                危险等级
                <n-slider :marks="marks" step="mark" :tooltip="false" v-model:value="levelFilter" />
            </div>

        </n-card>
        <n-list hoverable clickable bordered class="overflow-auto">
            <n-list-item v-for="message in store.analysisData.slice().reverse()" :key="message"
                v-show="shouldShow(message)" @click="displayDetail(message)">
                <n-thing content-style="margin-top: 10px;">
                    <template #description>
                        <n-space size="small" style="margin-top: 4px"
                            :class="{ 'text-red-500': message.tag.en == 'error' }">
                            <span class="text-base font-medium">{{ message.funcName }}</span>
                            <n-tag :bordered="false" :type="message.tag.en" size="small">
                                {{ message.tag.type }}
                            </n-tag>
                            <span>{{ message.tag.message }}</span>
                        </n-space>
                    </template>
                    <div>pid: {{ message.pid }}</div>
                    <div class="paramBox flex flex-row flex-wrap divide-x divide-gray-400">
                        <div v-for="param in message.params" :key="param" class="max-w-full break-all">
                            {{ param[0] }}: {{ param[1] }}
                        </div>
                    </div>
                    <div>返回: {{ message.result }}</div>
                </n-thing>
            </n-list-item>
        </n-list>
        <n-modal v-model:show="showModal" transform-origin="center">
            <n-card style="width: 600px" title="详细信息" :bordered="true" size="huge" role="dialog" aria-modal="true">
                <template #header-extra>
                    <div class="hover:bg-red-400 hover:text-white flex flex-row items-center cursor-pointer"
                        @click="showModal = false">
                        <n-icon size="1.5em" :component="Dismiss20Regular" />
                    </div>
                </template>
                <!-- 内容 -->
                <n-tabs type="line" >
                    <n-tab-pane name="源程序" tab="源程序">
                        <div class="flex flex-row">
                            <div class="w-1/4">文件名:</div>
                            <div class="w-3/4">{{ fileInfo.name }}</div>
                        </div>
                        <div class="flex flex-row" id="fileInfoPath">
                            <div class="w-1/4">路径:</div>
                            <div class="w-3/4 overflow-scroll select-all">{{ fileInfo.path }}</div>
                        </div>
                        <div class="flex flex-row">
                            <div class="w-1/4">大小:</div>
                            <div class="w-3/4">{{ fileInfo.size }}字节</div>
                        </div>
                        <div class="flex flex-row">
                            <div class="w-1/4">修改时间:</div>
                            <div class="w-3/4">{{ fileInfo.modTime }}</div>
                        </div>
                    </n-tab-pane>
                    <n-tab-pane name="详情" tab="详情">
                        暂无
                    </n-tab-pane>
                </n-tabs>
            </n-card>
        </n-modal>
    </div>
</template>

<style scoped>
.paramBox div:first-child {
    @apply pr-2;
}

.paramBox div:last-child:not(:first-child) {
    @apply pl-2;
}

.paramBox div:not(:first-child):not(:last-child) {
    @apply px-2;
}
#fileInfoPath ::-webkit-scrollbar{
    display: none;
}
</style>
