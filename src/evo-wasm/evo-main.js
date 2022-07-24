


const canvas = document.getElementById('env-canvas');
const canvas_ctx = canvas.getContext('2d');

function render() {
    let img = new ImageData(window.canvas_data.slice(), canvas.width, canvas.height);
    canvas_ctx.putImageData(img, 0, 0);
}

const myWorker = new Worker('./evo-worker.js');

myWorker.addEventListener("message", (msg) => {
    if(typeof msg === 'undefined' || !msg) return;
    if("memory" in msg.data) {
        window.canvas_data = new Uint8ClampedArray(msg.data["memory"], msg.data["ptr"], msg.data["nbytes"]);
    }
    else if("render" in msg.data) {
        // requestAnimationFrame(render)
        render()
    }
  
  });
  
myWorker.postMessage({"canvas-width": canvas.width, "canvas-height": canvas.height, "canvas-scale": 4})
