<script setup>
import { NSpace, NThing, NTag, NCard } from 'naive-ui';
const props = defineProps({
    index: {
        type: Number
    },
    source: {
        type: Object,
        default() {
            return {}
        }
    },
    shouldShow: {
        type: Function,
        default() {
            return () => true
        }
    },
    displayDetail: {
        type: Function,
        default() {
            return () => { }
        }
    }
});
</script>
<template>
    <n-card v-show="shouldShow(source)" @click="displayDetail(source)"
        class="cursor-pointer messageItemCard hover:bg-gray-100" size="large">
        <n-thing content-style="margin-top: 10px;">
            <template #description>
                <n-space size="small" style="margin-top: 4px" :class="{ 'text-red-500': source.tag.en == 'error' }">
                    <span class="text-base font-medium">{{ source.funcName }}</span>
                    <n-tag :bordered="false" :type="source.tag.en" size="small">
                        {{ source.tag.type }}
                    </n-tag>
                    <span>{{ source.tag.message }}</span>
                </n-space>
            </template>
            <div>{{ source.conclusion }}</div>
        </n-thing>
    </n-card>
</template>

<style>
</style>