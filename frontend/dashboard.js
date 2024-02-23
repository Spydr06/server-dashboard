const decodeCStr = (memBuffer, ptr, len) => new TextDecoder().decode(new Uint8Array(memBuffer, ptr, len));

const makeEnv = (env) => new Proxy(env, {
    get(target, prop, receiver) {
        if(env[prop] !== undefined) {
            return env[prop].bind(env);
        }
        return (...args) => {
            throw new Error(`NOT IMPLEMENTED: ${prop} ${args}`);
        }
    }
});

class Dashboard {
    constructor() {
        this.widgetsContainer = document.getElementById('widgets-container'); 
        this.currentWidget = undefined;
    }

    async start(wasm_file) {
        if(this.wasm !== undefined)
            throw new Error('DASHBOARD ALREADY STARTED.');
        this.wasm = await WebAssembly.instantiateStreaming(fetch(wasm_file), { env: makeEnv(this) });
        this.wasm.instance.exports.start();
        return this;
    }

    begin_widget(titlePtr, titleLen) {
        if(this.currentWidget !== undefined)
            throw new Error(`WIDGET ${this.currentWidget.title} IS STILL ACTIVE.`);

        const buffer = this.wasm.instance.exports.memory.buffer;
        this.currentWidget = {
            title: decodeCStr(buffer, titlePtr, titleLen),
            element: document.createElement('div') 
        };

        this.currentWidget.element.classList.add('widget');
        this.currentWidget.element.innerHTML = `<b class='widget-title'>${this.currentWidget.title}</b>`

        console.log(this.currentWidget);
    }

    end_widget() {
        this.widgetsContainer.appendChild(this.currentWidget.element);
        this.currentWidget = undefined;
    }
}

window.onload = async () => {
    const dashboard = await new Dashboard()
        .start('dashboard.wasm');
};

