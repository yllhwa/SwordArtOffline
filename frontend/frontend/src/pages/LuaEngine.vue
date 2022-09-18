<script setup>
import { NCode, NButton } from "naive-ui";
import { ReadFile, OpenFileByDefaultApplication } from '@/../wailsjs/go/main/App'
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
</template>

<style scoped>
</style>
