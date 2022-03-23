const { defineConfig } = require("@vue/cli-service");
module.exports = defineConfig({
  transpileDependencies: true,
  pluginOptions: {
    electronBuilder: {
      chainWebpackMainProcess: (config) => {
        config.module
          .rule("node")
          .test(/\.node$/)
          .use("node-loader")
          .loader("node-loader")
          .end();
      },
      mainProcessWatch: ["src/background.js"],
      preload: ["src/preload.js"],
    },
  },
});
