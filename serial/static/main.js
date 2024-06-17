import { ESPManager } from './esp.js';
import { bufferToString } from './utils.js';

function main() {
    const esp = new ESPManager();
    function wrap(fn) {
        return (...args) => fn(...args).catch(e => {
            console.error(e, fn.toString());
            espLoaderTerminal.writeLine(e.toString());
        });
    }

    async function program() {
        if (!connectBtn.hasAttribute('disabled'))
            return espLoaderTerminal.writeLine('Connect device to program.');

        const readWhenDone = esp.isReading;

        espLoaderTerminal.writeLine('Stopping reading');
        await esp.stopReading();

        espLoaderTerminal.writeLine('ESP Disconnect');
        await esp.disconnect();

        espLoaderTerminal.writeLine('ESP Setup');
        await esp.setup();

        espLoaderTerminal.writeLine('ESP Connect');
        await esp.connect({
            baudrate: 115200,
            terminal: espLoaderTerminal
        });

        [programBtn, disconnectBtn, consoleStartBtn, consoleStopBtn]
            .forEach(x => x.setAttribute('disabled', 'disabled'));

        espLoaderTerminal.clean();

        espLoaderTerminal.writeLine('Programming ESP32...')

        espLoaderTerminal.writeLine('Downloading binary');

        const req = await fetch('/download'),
            blob = await req.blob(),
            binaryData = await blob.arrayBuffer();

        espLoaderTerminal.writeLine(`Got binary, size is: ${blob.size}`);

        espLoaderTerminal.writeLine('Decoding to a string');

        const binaryString = bufferToString(binaryData);

        espLoaderTerminal.writeLine('Done, programming...');

        try {
            const flashOptions = {
                fileArray: [{
                    address: 0,
                    data: binaryString
                }],
                flashSize: "keep",
                eraseAll: false,
                compress: true,
                reportProgress: (percentageComplete) => {
                    progressBar.value = percentageComplete;
                }
            };

            await esp.program(flashOptions);
        } catch (e) {
            console.error(e);
            espLoaderTerminal.writeLine(`Error: ${e.toString()}`);
        } finally {
            if (readWhenDone) {
                esp.beginReading((d) => espLoaderTerminal.write(d));

                //await esp.resetDevice();
                consoleStopBtn.removeAttribute('disabled');
            } else {
                consoleStartBtn.removeAttribute('disabled');
            }

            [programBtn, disconnectBtn]
                .forEach(x => x.removeAttribute('disabled'));
        }
    }

    const socket = new WebSocket(`wss://${location.host}/ws`);

    socket.addEventListener('message', ({ data }) => {
        if (data === 'binary-change') {
            espLoaderTerminal.writeLine('Binary changed, begining programming');

            program();
        }
    });
    socket.addEventListener('error', ({ e }) => espLoaderTerminal.writeLine(e));

    const output = document.querySelector('#output'),
        connectBtn = document.querySelector('#connect'),
        disconnectBtn = document.querySelector('#disconnect'),
        consoleStartBtn = document.querySelector('#consoleStart'),
        consoleStopBtn = document.querySelector('#consoleStop'),
        programBtn = document.querySelector('#programBtn'),
        clearLog = document.querySelector('#clearLog'),
        progressBar = document.querySelector('#progress');

    const espLoaderTerminal = {
        clean(commit) {
            if (commit)
                output.textContent = '';
            else
                output.textContent += `\nESP request to clear console.\n\n`;
        },
        writeLine(data) {
            this.write(`${data}\n`);
        },
        write(data) {
            output.textContent += data;
        }
    };

    clearLog.addEventListener('click', () => espLoaderTerminal.clean(true));

    connectBtn.addEventListener('click', wrap(async (e) => {
        try {
            connectBtn.setAttribute('disabled', 'disabled');

            await esp.setup(() => {
                [consoleStartBtn, consoleStopBtn, programBtn, disconnectBtn]
                    .forEach(x => x.setAttribute('disabled', 'disabled'));

                connectBtn.removeAttribute('disabled');
            });

            const chip = await esp.connect({
                baudrate: 115200,
                terminal: espLoaderTerminal
            });

            espLoaderTerminal.writeLine(`Connected to: ${chip}`);

            [disconnectBtn, consoleStartBtn, programBtn]
                .forEach(x => x.removeAttribute('disabled'));
        } catch (e) {
            connectBtn.removeAttribute('disabled');

            throw e;
        }
    }));

    disconnectBtn.addEventListener('click', wrap(async () => {
        [consoleStartBtn, consoleStopBtn, programBtn, disconnectBtn]
            .forEach(x => x.setAttribute('disabled', 'disabled'));

        await esp.disconnect();

        connectBtn.removeAttribute('disabled');
    }));

    consoleStartBtn.addEventListener('click', wrap(async () => {
        consoleStartBtn.setAttribute('disabled', 'disabled');
        consoleStopBtn.removeAttribute('disabled');

        espLoaderTerminal.writeLine('Listening for serail data...\n');

        esp.beginReading((d) => espLoaderTerminal.write(d));
    }));

    consoleStopBtn.addEventListener('click', async () => {
        consoleStopBtn.setAttribute('disabled', 'disabled');

        await esp.stopReading();

        espLoaderTerminal.writeLine('ESP Disconnect');
        await esp.disconnect();

        espLoaderTerminal.writeLine('ESP Setup');
        await esp.setup();

        espLoaderTerminal.writeLine('ESP Connect');
        await esp.connect({
            baudrate: 115200,
            terminal: espLoaderTerminal
        });

        consoleStartBtn.removeAttribute('disabled');
    });

    programBtn.addEventListener('click', wrap(program));
}

document.addEventListener('DOMContentLoaded', main);