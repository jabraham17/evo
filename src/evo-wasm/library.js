
mergeInto(LibraryManager.library, {
    render: function (memory, nbytes) {
        // self.postMessage({"ptr": memory, "nbytes": nbytes});
        self.postMessage({"render": true});
        // // requestAnimationFrame(() => {
        // const img = new ImageData(canvas_data, canvas.width, canvas.height);
        // canvas_ctx.putImageData(img, 0, 0);
        // console.log(performance.now())
        // console.log(canvas, canvas_ctx)
        // // })
    },
});


