import { reactive, shallowReactive } from "vue";

const store = reactive({
  isInjecting: false,
  targetExe: "",
});

const analysisData = shallowReactive([]);
const memCacheData = shallowReactive([]);

export { store, analysisData, memCacheData };
