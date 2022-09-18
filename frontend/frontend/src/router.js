import {
  Home20Regular,
  DataPie20Regular,
  Settings20Regular,
  DataLine20Regular,
  Document20Regular,
  ChartMultiple20Regular,
} from "@vicons/fluent";
import { createRouter, createWebHashHistory } from "vue-router";

const routes = [
  {
    path: "/",
    name: "index",
    meta: {
      title: "主页",
    },
    icon: Home20Regular,
    component: () => import("./pages/Index.vue"),
  },
  {
    path: "/analysis",
    name: "analysis",
    meta: {
      title: "行为分析",
    },
    icon: DataPie20Regular,
    component: () => import("./pages/Analysis.vue"),
  },
  {
    path: "/meminfo",
    name: "meminfo",
    meta: {
      title: "内存分析",
    },
    icon: DataLine20Regular,
    component: () => import("./pages/Meminfo.vue"),
  },
  {
    path: "/stat",
    name: "stat",
    meta: {
      title: "统计",
    },
    icon: ChartMultiple20Regular,
    component: () => import("./pages/Stat.vue"),
  },
  {
    path: "/luaengine",
    name: "luaengine",
    meta: {
      title: "Lua脚本",
    },
    icon: Document20Regular,
    component: () => import("./pages/LuaEngine.vue"),
  },
  {
    path: "/setting",
    name: "setting",
    meta: {
      title: "设置",
    },
    icon: Settings20Regular,
    component: () => import("./pages/Setting.vue"),
  },
];

const router = createRouter({
  history: createWebHashHistory(),
  routes,
});

export default router;
export { routes };
