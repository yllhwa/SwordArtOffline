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
      title: "测试",
    },
    component: () => import("./pages/Analysis.vue"),
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
