import { reactive } from "vue";

const store = reactive({
  analysisData: [],
  isInjecting: false,
  targetExe: "",
});

export { store };
