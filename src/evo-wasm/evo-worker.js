importScripts("./evo.js")

let modPromise = Mod()

function run_worker(args) {
    modPromise.then((mod) => {
    mod._set_canvas_size(args["canvas-width"], args["canvas-height"], args["canvas-scale"]);
    self.postMessage({"memory": mod.wasmMemory.buffer, "ptr":  mod._get_canvas_memory(), "nbytes": mod._get_canvas_memory_nbytes()});
        // const canvas_ptr = mod._get_canvas_memory();
        // self.canvas_data = new Uint8ClampedArray(mod.asm.memory.buffer, canvas_ptr, args["canvas-width"] * args["canvas-height"] * 4);

        //     console.log(canvas_ptr)
        mod._main()
    })
}

self.onmessage = (msg) => {
    if(typeof msg === 'undefined' || !msg) return;
    run_worker(msg.data);
}

// self.canvas = document.getElementById('env-canvas');
// self.canvas_ctx = canvas.getContext('2d');

// Module().then(function (mod) {
//     // const int_sqrt = M.cwrap('int_sqrt', 'number', ['number']);
//     // console.log(int_sqrt(64));
//     console.log(mod);

//     mod._set_canvas_size(canvas.width, canvas.height, 8);
//     const canvas_ptr = mod._get_canvas_memory();
//     self.canvas_data = new Uint8ClampedArray(mod.asm.memory.buffer, canvas_ptr, canvas.width * canvas.height * 4);

//     // const img = new ImageData(canvas_data, canvas.width, canvas.height);
//     // ctx.putImageData(img, 0, 0);
//     // console.log(canvas_data[0])

//     mod._main()
// });
