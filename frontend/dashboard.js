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

const widgetClasses = [
    { cssClass: 'widget-generic', container: 'generic-widget-container' },
    { cssClass: 'widget-quicksetting', container: 'quicksetting-widget-container' },
    { cssClass: 'widget-service', container: 'service-widget-container' },
];

class Dashboard {
    constructor() {
    }

    async start(wasm_file) {
        if(this.wasm !== undefined)
            throw new Error('DASHBOARD ALREADY STARTED.');
        this.wasm = await WebAssembly.instantiateStreaming(fetch(wasm_file), { env: makeEnv(this) });
        this.wasm.instance.exports.start();
        return this;
    }

    newWidgetElement(id, title, widgetClass) {
        let el = document.createElement('div');
        el.classList.add('widget');
        el.classList.add(widgetClasses[widgetClass].cssClass);
        el.id = `widget-${id}`;
        el.innerHTML = `<b class='widget-title'>${title}</b><div id='widget-content-${id}' class='widget-content ${widgetClasses[widgetClass].cssClass}-content'></div>`;
        return el;
    }

    create_widget(id, titlePtr, titleLen, widgetClass) {
        const buffer = this.wasm.instance.exports.memory.buffer; 
        document.getElementById(widgetClasses[widgetClass].container).appendChild(this.newWidgetElement(id, decodeCStr(buffer, titlePtr, titleLen), widgetClass))
    }

    update_widget_content(id, htmlPtr, htmlLen) {
        const content = document.getElementById(`widget-content-${id}`);
        if(content === null)
            throw new Error(`No widget with id ${id}.`);

        const buffer = this.wasm.instance.exports.memory.buffer;
        content.innerHTML = decodeCStr(buffer, htmlPtr, htmlLen);
    }
}

window.onload = async () => {
    const dashboard = await new Dashboard()
        .start('dashboard.wasm');
};

