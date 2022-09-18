<script setup>
import { NButton } from "naive-ui";
import * as echarts from 'echarts/core';
import {
    TooltipComponent,
    GridComponent,
    LegendComponent
} from 'echarts/components';
import { BarChart } from 'echarts/charts';
import { CanvasRenderer } from 'echarts/renderers';
import { onMounted } from "@vue/runtime-core";
import { funTypeReverseMap } from "@/utils.js";
import { analysisData } from "@/store.js";
echarts.use([
    TooltipComponent,
    GridComponent,
    LegendComponent,
    BarChart,
    CanvasRenderer
]);

let countDom;
let countChart;
let countOption;
countOption = {
    tooltip: {
        trigger: 'axis',
        axisPointer: {
            type: 'shadow'
        }
    },
    legend: {},
    grid: {
        containLabel: true
    },
    xAxis: {
        type: 'value'
    },
    yAxis: {
        type: 'category',
        data: ['弹窗', '文件操作', '堆操作', '注册表操作', '网络操作']
    },
    color: [
        "#91cc75", "#fac858", "#ee6666"
    ],
    series: [
        {
            name: '正常',
            type: 'bar',
            stack: 'total',
            emphasis: {
                focus: 'series'
            },
            data: [0, 0, 0, 0, 0, 0, 0],
        },
        {
            name: '可疑',
            type: 'bar',
            stack: 'total',
            emphasis: {
                focus: 'series'
            },
            data: [0, 0, 0, 0, 0, 0, 0],
        },
        {
            name: '危险',
            type: 'bar',
            stack: 'total',
            emphasis: {
                focus: 'series'
            },
            data: [0, 0, 0, 0, 0, 0, 0],
        },
    ]
};
let caculateCount = () => {
    const indexMap = {
        弹窗: 0,
        文件操作: 1,
        堆操作: 2,
        注册表操作: 3,
        网络操作: 4,
    }
    // 清空
    countOption.series[0].data = [0, 0, 0, 0, 0, 0, 0];
    countOption.series[1].data = [0, 0, 0, 0, 0, 0, 0];
    countOption.series[2].data = [0, 0, 0, 0, 0, 0, 0];
    // 遍历store.analysisData
    analysisData.forEach((item) => {
        switch (item.tag.type) {
            case "正常":
                countOption.series[0].data[indexMap[funTypeReverseMap[item.funcName]]]++;
                break;
            case "可疑":
                countOption.series[1].data[indexMap[funTypeReverseMap[item.funcName]]]++;
                break;
            case "危险":
                countOption.series[2].data[indexMap[funTypeReverseMap[item.funcName]]]++;
                break;
        }
    });
}
onMounted(
    () => {
        caculateCount();
        countDom = document.getElementById('stat');
        countChart = echarts.init(countDom);
        countOption && countChart.setOption(countOption);
    }
)

let onRefreshClick = () => {
    caculateCount();
    countChart.setOption(countOption);
}
</script>

<template>
    <div class="py-2 flex flex-col h-full">
        <div>
            <n-button @click="onRefreshClick">刷新</n-button>
        </div>
        <div id="stat" class="w-full h-full"></div>
    </div>
</template>

<style scoped>
</style>
