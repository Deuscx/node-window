import { ipcRenderer, contextBridge } from "electron";
import { InvokeChannel } from "./constants";
contextBridge.exposeInMainWorld("api", {
  desktopCapturer(options) {
    return ipcRenderer.invoke(InvokeChannel.captureSource, options);
  },
});
