const activeWindow = require('active-win')
const addon = require('./build/Release/wm.node');
activeWindow().then((window) => {
    console.log(window.id, window.title)
    addon.start(window.id,(data)=>{
        console.log(data)
    })
})

setTimeout(() => {
    console.log('stop')
    addon.stop()
}, 10000);