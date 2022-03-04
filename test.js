const activeWindow = require('active-win')
const addon = require('./build/Release/wm.node');
setTimeout(() => {
    activeWindow().then((window) => {
        console.log(window.id, window.title)
        // addon.start(window.id,(data)=>{
        //     console.log(data)
        // })
       setTimeout(() => {
        console.log('focusWindow')
        addon.focusWindow(window.id)
       }, 3000);
    })
}, 3000);

// setTimeout(() => {
//     console.log('stop')
//     addon.stop()
// }, 10000);