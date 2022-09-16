<script setup>
import { NCode, NButton } from "naive-ui";
import { NIcon } from "naive-ui";
import { Document20Regular } from "@vicons/fluent";
import { ReadFile, OpenFileByDefaultApplication } from '../../wailsjs/go/main/App'
let luaFileContent = $ref("");
ReadFile("LuaEngine.lua").then(content => {
    luaFileContent = content;
})

let onRefreshClick = () => {
    ReadFile("LuaEngine.lua").then(content => {
        luaFileContent = content;
    })
}

let onEditClick = () => {
    OpenFileByDefaultApplication("LuaEngine.lua");
}
</script>

<template>
    <div class="px-6 py-2">
        <div class="text-2xl py-4 flex flex-row items-center">
            <n-icon size="1.25em" :component="Document20Regular" />
            <span class="px-2">Lua脚本</span>
        </div>
        <div class="py-3 flex flex-row gap-2">
            <div>
                <n-button @click="onRefreshClick">刷新</n-button>
            </div>
            <div>
                <n-button @click="onEditClick">编辑</n-button>
            </div>
        </div>
        <div class="overflow-auto h-4/5">
            <n-code :code="luaFileContent" language="lua" show-line-numbers />
        </div>
    </div>
</template>

<style scoped>
</style>
