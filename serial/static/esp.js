import { ESPLoader, Transport } from './esptool.js'

function main() {
    const output = document.querySelector('#output'),
        connectBtn = document.querySelector('#connect'),
        disconnectBtn = document.querySelector('#disconnect'),
        consoleStartBtn = document.querySelector('#consoleStart'),
        consoleStopBtn = document.querySelector('#consoleStop');

    /** Serial Device */
    let device,
        transport,
        esploader,
        isConsoleClosed = true;

    function wrap(fn) {
        return (...args) => fn(...args).catch(e => espLoaderTerminal.writeLine(e.toString()));
    }

    const espLoaderTerminal = {
        clean() {
        },
        writeLine(data) {
            output.textContent += `${data}\n`;
        },
        write(data) {
            output.textContent += data;
        }
    };

    connectBtn.addEventListener('click', wrap(async (e) => {
        if (!device) {
            device = await navigator.serial.requestPort({});
            transport = new Transport(device, true);
        }

        const flashOptions = {
            transport,
            baudrate: 115200,
            terminal: espLoaderTerminal,
        };
        esploader = new ESPLoader(flashOptions);
    
        const chip = await esploader.main();

        espLoaderTerminal.writeLine(`Connected to: ${chip}`);
    }));

    disconnectBtn.addEventListener('click', wrap(async (e) => {
        if (transport) await transport.disconnect();

        transport = device = esploader = undefined;

        espLoaderTerminal.writeLine("Disconnected...");
    }));

    consoleStartBtn.addEventListener('click', wrap(async () => {
        if (!device) {
            device = await navigator.serial.requestPort({});
            transport = new Transport(device, true);
        }

        await transport.connect(115200);
        isConsoleClosed = false;

        while (true && !isConsoleClosed) {
            const val = await transport.rawRead();
            if (typeof val !== "undefined") {
              term.write(val);
            } else {
              break;
            }
          }
    }));
}

document.addEventListener('DOMContentLoaded', main);