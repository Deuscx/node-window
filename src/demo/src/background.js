import {
  app,
  protocol,
  BrowserWindow,
  screen,
  desktopCapturer,
  ipcMain,
} from "electron";
import { createProtocol } from "vue-cli-plugin-electron-builder/lib";
import path from "path";
import { InvokeChannel, SendChannel } from "./constants";
import installExtension, { VUEJS3_DEVTOOLS } from "electron-devtools-installer";
const isDevelopment = process.env.NODE_ENV !== "production";

// Scheme must be registered before the app is ready
protocol.registerSchemesAsPrivileged([
  { scheme: "app", privileges: { secure: true, standard: true } },
]);

const addon = require("../../../build/Release/wm.node");

const winMap = new Map();
async function createWindow() {
  // Create the browser window.
  const win = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.resolve(__dirname, "./preload.js"),
    },
  });

  if (process.env.WEBPACK_DEV_SERVER_URL) {
    // Load the url of the dev server if in development mode
    try {
      // await installExtension(VUEJS3_DEVTOOLS);
    } catch (error) {
    }
    await win.loadURL(process.env.WEBPACK_DEV_SERVER_URL);
    win.webContents.openDevTools({ mode: "detach" });
  } else {
    createProtocol("app");
    // Load the index.html when not in development
    win.loadURL("app://./index.html");
  }
  winMap.set("main", win);
  createOverlayWindow();
}

function updateOverlayBounds(bounds, win) {
  let lastBounds = bounds;
  if (process.platform === "win32") {
    lastBounds = screen.screenToDipRect(win, bounds);
  }
  win.setBounds(lastBounds);
}
// Quit when all windows are closed.
app.on("window-all-closed", () => {
  // On macOS it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== "darwin") {
    app.quit();
  }
});

app.on("activate", () => {
  // On macOS it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (BrowserWindow.getAllWindows().length === 0) createWindow();
});

app.on("ready", async () => {
  createWindow();
});

function createOverlayWindow() {
  const createOverlayOptions = {
    frame: false,
    show: true,
    transparent: true,
    // let Chromium to accept any size changes from OS
    resizable: true,
    // disable shadow for Mac OS
    hasShadow: false,
  };

  const win = new BrowserWindow(createOverlayOptions);
  win.loadURL(`data:text/html;charset=utf-8,
  <body style="padding: 0; margin: 0;">
    <div style="position: absolute; width: 100vw; height: 100vh; border: 3px solid rgba(0, 225, 90, 1); box-sizing: border-box;">
    </div>
  </body>`);
  win.setIgnoreMouseEvents(true);
  // win.setAlwaysOnTop(true)
  winMap.set("overlay", win);
  return win;
}

ipcMain.handle(InvokeChannel.captureSource, async (event, options) => {
  const sources = await desktopCapturer.getSources(options);
  return sources;
});

ipcMain.on(SendChannel.start, (event, id) => {
  const main = winMap.get("main");
  const overlay = winMap.get("overlay");
  const hwnd = main.getNativeWindowHandle();
  if (!isScreen(id)) {
    const target = id.split(":")[1];
    let numberTarget = Number(target);
    console.log("🚀 numberTarget", numberTarget);
    addon.focusWindow(numberTarget);
    updateOverlayBounds(addon.getWindowPosition(numberTarget), overlay);
    addon.start(numberTarget, 
      // overlay.getNativeWindowHandle(), 
      (data) => {
      if (data.type === 3) {
        // console.log("🚀data.position", data.position)
        updateOverlayBounds(data.position, overlay);
      }
    });
  }
});

ipcMain.handle(SendChannel.stop, (event) => {
  console.log("🚀 ~  stop");
  addon.stop();
});

/**
 *
 * @param {string} id
 * @returns
 */
function isScreen(id) {
  return id.startsWith("screen");
}
