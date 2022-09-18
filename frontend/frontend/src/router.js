import { createRouter, createWebHashHistory } from "vue-router";
import {
  Home20Regular,
  DataPie20Regular,
  Settings20Regular,
  DataLine20Regular,
  Document20Regular,
  ChartMultiple20Regular,
} from "@vicons/fluent";
import Index from "./pages/Index.vue";
import Analysis from "./pages/Analysis.vue";
import Meminfo from "./pages/Meminfo.vue";
import Stat from "./pages/Stat.vue"
import LuaEngine from "./pages/LuaEngine.vue"
import Setting from "./pages/Setting.vue"

const routes = [
  {
    path: "/",
    name: "index",
    meta: {
      title: "主页",
      icon: Home20Regular,
    },
    component: Index,
  },
  {
    path: "/analysis",
    name: "analysis",
    meta: {
      title: "行为分析",
      icon: DataPie20Regular,
    },
    component: Analysis,
  },
  {
    path: "/meminfo",
    name: "meminfo",
    meta: {
      title: "内存分析",
      icon: DataLine20Regular,
    },
    component: Meminfo,
  },
  {
    path: "/stat",
    name: "stat",
    meta: {
      title: "统计",
      icon: ChartMultiple20Regular,
    },
    component: Stat,
  },
  {
    path: "/luaengine",
    name: "luaengine",
    meta: {
      title: "Lua脚本",
      icon: Document20Regular,
    },
    component: LuaEngine,
  },
  {
    path: "/setting",
    name: "setting",
    meta: {
      title: "设置",
      icon: Settings20Regular,
    },
    component: Setting,
  },
];

const router = createRouter({
  history: createWebHashHistory(),
  routes,
});

export default router;
export { routes };
