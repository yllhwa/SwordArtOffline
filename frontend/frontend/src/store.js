import { reactive } from "vue";

const store = reactive({
  analysisData: [],
  memCacheData: [],
  isInjecting: false,
  targetExe: "",
});

export { store };
