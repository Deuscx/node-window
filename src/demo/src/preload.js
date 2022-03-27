import { ipcRenderer, contextBridge } from "electron";
import { InvokeChannel, SendChannel } from "./constants";
contextBridge.exposeInMainWorld("api", {
  desktopCapturer(options) {
    return ipcRenderer.invoke(InvokeChannel.captureSource, options);
  },
  start(id){
    return ipcRenderer.send(SendChannel.start, id)
  },
  stop(){
    return ipcRenderer.invoke(SendChannel.stop)
  }
});