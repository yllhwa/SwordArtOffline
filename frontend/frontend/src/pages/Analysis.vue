<script setup>
import { NCard, NCheckboxGroup, NCheckbox, NSpace, NList, NListItem, NThing, NTag, NIcon, NSlider, NModal } from 'naive-ui';
import { NTabs, NTabPane } from 'naive-ui';
import { DataPie20Regular, Dismiss20Regular } from '@vicons/fluent';
import { store } from '@/store.js';
import { GetFileInfoByPid } from '@/../wailsjs/go/main/App'
import VirtualList from 'vue3-virtual-scroll-list';
import MessageItem from "@/components/MessageItem.vue";
import { shouldShowTrack, funcTypeMap, funTypeReverseMap, getOperationCacheByFunc } from '../utils.js';

// 按类别筛选
let dataFilter = $ref(["弹窗", "文件操作", "堆操作", "注册表操作", "网络操作"]);
let dataFilterList = $ref(["弹窗", "文件操作", "堆操作", "注册表操作", "网络操作"]);
let isCheckAll = $ref(true);
let onChooseAllChecked = () => {
    dataFilter = isCheckAll ? [] : dataFilterList;
}

// 按威胁等级筛选
let levelFilter = $ref(100);
let marks = {
    0: "危险",
    50: "可疑",
    100: "全部",
}
let shouldShow = (message) => {
    // 先判断威胁等级
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
    // 再判断类别
    return dataFilter.includes(funTypeReverseMap[message.funcName]);
}

// 详细信息模态框
let showModal = $ref(false);
let fileInfo = $ref({
    Name: "",
    Path: "",
    Size: 0,
    ModTime: "",
});
let funcDetailInfo = $ref({});

let infoByPidCache = {};
let displayDetail = (message) => {
    showModal = true;
    let pid = parseInt(message.pid);
    if (infoByPidCache[pid]) {
        fileInfo = infoByPidCache[pid];
    } else {
        GetFileInfoByPid(pid).then((info) => {
            infoByPidCache[pid] = info;
            fileInfo = info;
        });
    }
    funcDetailInfo = message;
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
            <div class="mt-2 w-1/5">
                危险等级
                <n-slider :marks="marks" step="mark" :tooltip="false" v-model:value="levelFilter" />
            </div>
        </n-card>
        <virtual-list style="overflow-y: auto;" :data-key="'id'" :data-sources="store.analysisData.slice().reverse()"
            :estimate-size="111" :data-component="MessageItem"
            :extra-props="{ shouldShow: shouldShow, displayDetail: displayDetail }" />
        <n-modal v-model:show="showModal" transform-origin="center" class="mx-10">
            <n-card style="width: 600px" title="详细信息" :bordered="true" size="huge" role="dialog" aria-modal="true">
                <template #header-extra>
                    <div class="hover:bg-red-400 hover:text-white flex flex-row items-center cursor-pointer"
                        @click="showModal = false">
                        <n-icon size="1.5em" :component="Dismiss20Regular" />
                    </div>
                </template>
                <!-- 内容 -->
                <n-tabs type="line">
                    <n-tab-pane name="详情" tab="详情">
                        <table class="table-auto">
                            <tbody>
                                <tr class="py-2">
                                    <td class="pr-6 whitespace-nowrap">函数名:</td>
                                    <td>{{ funcDetailInfo.funcName }}</td>
                                </tr>
                                <tr class="py-2">
                                    <td class="pr-6 whitespace-nowrap">返回值:</td>
                                    <td>{{ funcDetailInfo.result }}</td>
                                </tr>
                                <tr class="py-2" v-for="param in funcDetailInfo.params" :key="param">
                                    <td class="pr-6 whitespace-nowrap">{{ param.name }}:</td>
                                    <td class="whitespace-pre-wrap">{{ param.value }}</td>
                                </tr>
                            </tbody>
                        </table>
                    </n-tab-pane>
                    <n-tab-pane name="源程序" tab="源程序">
                        <table class="table-auto">
                            <tbody>
                                <tr class="py-2">
                                    <td class="pr-3">文件名:</td>
                                    <td>{{ fileInfo.name }}</td>
                                </tr>
                                <tr class="py-2">
                                    <td class="pr-3">路径:</td>
                                    <td>{{ fileInfo.path }}</td>
                                </tr>
                                <tr class="py-2">
                                    <td class="pr-3">大小:</td>
                                    <td>{{ fileInfo.size }}字节</td>
                                </tr>
                                <tr class="py-2">
                                    <td class="pr-3">修改时间:</td>
                                    <td>{{ fileInfo.modTime }}</td>
                                </tr>
                            </tbody>
                        </table>
                    </n-tab-pane>
                    <n-tab-pane name="追踪日志" tab="追踪日志" v-if="shouldShowTrack(funcDetailInfo)">
                        <div v-for="(oper, index) in getOperationCacheByFunc(funcDetailInfo)?.operation" :key="index">
                            {{ index + 1 }}:{{ oper }}
                        </div>
                    </n-tab-pane>
                </n-tabs>
            </n-card>
        </n-modal>
    </div>
</template>

<style>
.paramBox div:first-child {
    @apply pr-2;
}

.paramBox div:last-child:not(:first-child) {
    @apply pl-2;
}

.paramBox div:not(:first-child):not(:last-child) {
    @apply px-2;
}

#fileInfoPath ::-webkit-scrollbar {
    display: none;
}

.n-modal,
.n-modal>.n-card-header,
.n-modal>.n-card__content {
    width: 1000px !important;
    max-width: 90vw !important;
    max-height: 95vh !important;
    /* @apply overflow-x-hidden overflow-y-auto; */
}

.n-tab-pane {
    max-height: 60vh !important;
    @apply overflow-x-hidden overflow-y-auto;
}
</style>
