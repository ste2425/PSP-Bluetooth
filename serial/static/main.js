import { bufferToString } from './utils.js';
import { ESPConnectionFactoryService } from './services/espconnection-factory.service.js';
import { SerialConnectionFactoryService } from './services/serial-connection-factory.service.js';

function main() {
    const espServiceFactory = new ESPConnectionFactoryService(new SerialConnectionFactoryService());
    let espSerialService,
        espProgrammingService;
    function wrap(fn) {
        return (...args) => fn(...args).catch(e => {
            console.error(e, fn.toString());
            espLoaderTerminal.writeLine(e.toString());
        });
    }

    async function program() {
        try {
            if (espSerialService.connected) {            
                espLoaderTerminal.writeLine('Disconnecting from serial');

                await espSerialService.disconnect();
            } else {
                espLoaderTerminal.writeLine('Serial not connected');
            }
            
            espLoaderTerminal.writeLine('connecting programmer');

            espProgrammingService = await espServiceFactory.programmingConnection();

            await espProgrammingService.connect({
                baudrate: 921600,
                terminal: espLoaderTerminal
            });

            espLoaderTerminal.writeLine('connected');

            espLoaderTerminal.writeLine('Downloading binary');

            const req = await fetch('/download'),
                blob = await req.blob(),
                binaryData = await blob.arrayBuffer();

            espLoaderTerminal.writeLine(`Got binary, size is: ${blob.size}`);

            espLoaderTerminal.writeLine('Decoding to a string');

            const binaryString = bufferToString(binaryData);

            espLoaderTerminal.writeLine('Done, programming...');

            const flashOptions = {
                fileArray: [{
                    address: 0,
                    data: binaryString
                }],
                flashSize: "keep",
                eraseAll: false,
                compress: true,
                reportProgress: (fileIndex, written, total) => {
                    progressBar.value = (written / total) * 100;
                }
            };

            await espProgrammingService.program(flashOptions);
            
            await espProgrammingService.disconnect();

            espLoaderTerminal.writeLine('Disconnected');

            progressBar.value = 0;

            await new Promise(res => setTimeout(res, 2000));

            espSerialService = await espServiceFactory.serialConnection();

            espLoaderTerminal.writeLine('Connecting to device');

            await espSerialService.connect();

            espSerialService.startReading(d => espLoaderTerminal.write(d));

            await espSerialService.reset();
        } catch (e) {
            console.error(e);
            espLoaderTerminal.writeLine(`Error: ${e.toString()}`);

            if (espProgrammingService.connected)
                await espProgrammingService.disconnect();
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
        clearLog = document.querySelector('#clearLog'),
        progressBar = document.querySelector('#progress'),
        scrollContainer = document.querySelector('#scrollContainer');

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
            scrollContainer.scrollTop = scrollContainer.scrollHeight;
        }
    };

    clearLog.addEventListener('click', () => espLoaderTerminal.clean(true));

    connectBtn.addEventListener('click', wrap(async (e) => {
        try {
            connectBtn.setAttribute('disabled', 'disabled');

            espLoaderTerminal.writeLine('Creating connection instance');

            espSerialService = await espServiceFactory.serialConnection();

            espLoaderTerminal.writeLine('Connecting to device');

            await espSerialService.connect();

            await espSerialService.startReading(d => espLoaderTerminal.write(d));
        } catch (e) {
            connectBtn.removeAttribute('disabled');

            espLoaderTerminal.writeLine('Error connecting');
            espLoaderTerminal.writeLine(e.toString());

            throw e;
        }
    }));
}

document.addEventListener('DOMContentLoaded', main);