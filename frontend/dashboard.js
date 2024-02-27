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

const E_SUCCESS = 0;
const E_404 = 1;
const E_NULL = 2;

const errorCodes = [
    'Success',
    '404 Response',
    '(null)'
];

class Dashboard {
    async start(wasm_file) {
        if(this.wasm !== undefined)
            throw new Error('DASHBOARD ALREADY STARTED.');
        this.wasm = await WebAssembly.instantiateStreaming(fetch(wasm_file), { env: makeEnv(this) });
        this.wasm.instance.exports.start();
        this.update();
        return this;
    }

    update() {
        const err = this.wasm.instance.exports.update();
        if(err !== 0)
            console.log(`Error updating: ${errorCodes[err]} (${err})`);
    }

    set_inner_html(elementIdPtr, elementIdLen, htmlPtr, htmlLen) {
        const buffer = this.wasm.instance.exports.memory.buffer;
        
        const content = document.getElementById(decodeCStr(buffer, elementIdPtr, elementIdLen));
        if(content === null)
            return E_NULL;

        content.innerHTML = decodeCStr(buffer, htmlPtr, htmlLen);
    }

    async api_request(endpointPtr, endpointLen, responseBufferPtr, responseBufferLen) {
        const buffer = this.wasm.instance.exports.memory.buffer;
        const endpoint = decodeCStr(buffer, endpointPtr, endpointLen);

        try {
            const response = await fetch(endpoint, { method: 'GET', cache: 'no-cache', redirect: 'follow' });
            if(!response.ok)
                return E_404;
            const content = await response.blob();
            
            const reader = new FileReader();
            reader.onload = () => {
                const srcBuffer = new Uint8Array(reader.result, 0, Math.min(responseBufferLen, content.size));
                const destBuffer = new Uint8Array(buffer, responseBufferPtr, responseBufferLen);
                destBuffer.set(srcBuffer);
            }

            await reader.readAsArrayBuffer(content);
            return E_SUCCESS;
        }
        catch(error) {
            throw new Error(`GET ${endpoint}: ${error}`);
            return E_404;
        }
    }
}

const updateInterval = 1000;

window.onload = async () => {
    const dashboard = await new Dashboard()
        .start('dashboard.wasm');

    setInterval(() => dashboard.update(), updateInterval);
};

