<script setup>
import { NInput, NButton, NInputGroup } from "naive-ui";
import { NIcon } from "naive-ui";
import { AppGeneric20Regular, Syringe20Regular } from "@vicons/fluent";
import { OpenExeDialog, InjectExe } from "@/../wailsjs/go/main/App"
import { store, analysisData } from "@/store";
import router from "@/router";

let onSelectExeBtnClick = () => {
    OpenExeDialog().then(exe => {
        store.targetExe = exe;
    })
}

let onInjectClick = () => {
    if (store.isInjecting) {
        store.isInjecting = false;
    } else {
        // 清空原有数据
        analysisData.value = [];
        // 标记正在注入
        store.isInjecting = true;
        InjectExe(store.targetExe);
        // 切换到分析页
        router.push("/analysis");
    }

}
</script>

<template>
    <div class="py-2">
        <div class="text-base font-medium">待分析可执行文件</div>
        <div class="flex flex-row mt-2">
            <n-input-group>
                <n-input placeholder="选择待分析可执行文件" v-model:value="store.targetExe" clearable>
                    <template #prefix>
                        <n-icon :component="AppGeneric20Regular" />
                    </template>
                </n-input>
                <n-button ghost class="px-2" @click="onSelectExeBtnClick">选择</n-button>
            </n-input-group>
        </div>
    </div>
    <div class="flex flex-col justify-center items-center py-2">
        <n-button size="large" strong @click="onInjectClick" :type="store.isInjecting ? `error` : ``">
            <template #icon>
                <n-icon :component="Syringe20Regular" />
            </template>
            <span>{{ store.isInjecting ? `停止注入` : `开始注入` }}</span>
        </n-button>
    </div>
</template>

<style scoped>
</style>
