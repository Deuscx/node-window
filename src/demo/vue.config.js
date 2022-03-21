const { defineConfig } = require('@vue/cli-service')
module.exports = defineConfig({
  transpileDependencies: true,
  pluginOptions: {
    electronBuilder: {
      mainProcessWatch: ['src/background.js'],
      preload: ['src/preload.js']
    }
  }
})
