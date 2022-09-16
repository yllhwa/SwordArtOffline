import { createRouter, createWebHashHistory } from "vue-router";

const routes = [
  {
    path: "/",
    redirect: "index",
  },
  {
    path: "/index",
    name: "index",
    meta: {
      title: "主页",
    },
    component: () => import("./pages/Index.vue"),
  },
  {
    path: "/analysis",
    name: "analysis",
    meta: {
      title: "分析",
    },
    component: () => import("./pages/Analysis.vue"),
  },
  {
    path: "/meminfo",
    name: "meminfo",
    meta: {
      title: "内存信息",
    },
    component: () => import("./pages/Meminfo.vue"),
  },
  {
    path: "/luaengine",
    name: "luaengine",
    meta: {
      title: "Lua脚本",
    },
    component: () => import("./pages/LuaEngine.vue"),
  },
  {
    path: "/setting",
    name: "setting",
    meta: {
      title: "设置",
    },
    component: () => import("./pages/Setting.vue"),
  },
];

const router = createRouter({
  history: createWebHashHistory(),
  routes,
});

export default router;
