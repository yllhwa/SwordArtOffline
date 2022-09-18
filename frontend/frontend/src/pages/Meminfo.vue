<script setup>
import { NIcon } from "naive-ui";
import { DataLine20Regular } from "@vicons/fluent";
import { store } from "@/store.js";
import * as echarts from 'echarts/core';
import { GraphChart } from 'echarts/charts';
import { CanvasRenderer } from 'echarts/renderers';
import { onMounted } from "@vue/runtime-core";
let lastLen = 0;
let data = $ref([]);
let edges = $ref([]);
let chartDom, myChart, option;
option = {
    animation: false,
    series: [
        {
            type: 'graph',
            layout: 'force',
            animation: false,
            data: data,
            roam: true,
            force: {
                // initLayout: 'circular'
                // gravity: 0
                repulsion: 100,
                edgeLength: 5,
                // 箭头
                edgeSymbol: ['circle', 'arrow'],

                // initLayout: 'circular',
                // repulsion: 100,
                // gravity: 0.1,
                // edgeLength: 100,
                // layoutAnimation: false
            },
            edges: edges,
            edgeSymbol: ['circle', 'arrow'],
            edgeSymbolSize: [4, 10],
        },

    ],
};
onMounted(() => {
    echarts.use([GraphChart, CanvasRenderer]);
    chartDom = document.getElementById('main');
    myChart = echarts.init(chartDom);
    setInterval(function () {
        // 长度和上次相同则不更新
        if (store.memCacheData.length == lastLen) {
            return;
        }
        lastLen = store.memCacheData.length;
        // 从缓存中取出数据
        // 从上次的最后一个开始
        let len = store.memCacheData.length;
        for (let i = 0; i < len; i++) {
            let _data = store.memCacheData[i];
            // console.log(_data);
            // 从参数中取出src和dst
            let src = _data.params[0].value;
            let dst = _data.params[1].value;
            // console.log(src, dst);
            // 若src和dst都不在data中则添加
            if (!data.find((item) => item.name == src)) {
                data.push({
                    name: src,
                    symbolSize: 10,
                    draggable: true
                });
            }
            if (!data.find((item) => item.name == dst)) {
                data.push({
                    name: dst,
                    symbolSize: 10,
                    draggable: true
                });
            }
            // 若src和dst不在edges中则添加
            if (!edges.find((item) => item.source == src && item.target == dst)) {
                edges.push({
                    source: src,
                    target: dst
                });
            }
        }
        myChart.setOption(option);
    }, 200)
})

</script>

<template>
    <div class="px-6 py-2">
        <div class="text-2xl py-4 flex flex-row items-center">
            <n-icon size="1.25em" :component="DataLine20Regular" />
            <span class="px-2">内存分析</span>
        </div>
        <div class="py-2 flex flex-col h-full" id="main">
        </div>
    </div>
</template>

<style scoped>
</style>
