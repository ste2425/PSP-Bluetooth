import serialDevice from './serialDevice.js';
import { ESPLoader, Transport } from './esptool.js';
    
const decoder = new TextDecoder();

export class ESPManager {
    #device;
    #transport;
    #espLoader;
    #isConsoleClosed = true;
    #onDisconnect;
    #reader;

    async resetDevice() {
        if (!this.#transport)
            return;

        await this.#transport.setDTR(false);
        await new Promise((resolve) => setTimeout(resolve, 100));
        await this.#transport.setDTR(true);
    }

    async setup(onDisconnect = () => {}) {
        this.#onDisconnect = onDisconnect;

        this.#device = await serialDevice.connect();
        this.#device.addEventListener('disconnect', this.#deviceDisconnect);

        this.#transport = new Transport(this.#device, false);
    }

    async connect(options) {
        const flashOptions = {
            transport: this.#transport,
            ...options
        };
        this.#espLoader = new ESPLoader(flashOptions);
    
        return await this.#espLoader.main();
    }

    async disconnect() {        
        await serialDevice.releaseReaders();

        await this.#transport.disconnect();

        this.#deviceDisconnect();
    }

    async program(options) {
        const flashOptions = {
            ...options,
            reportProgress: (fileIndex, written, total) => {
                options.reportProgress((written / total) * 100);
            },
            calculateMD5Hash: (image) => CryptoJS.MD5(CryptoJS.enc.Latin1.parse(image))
        };
        await this.#espLoader.writeFlash(flashOptions);
    }

    async beginReading(onData) {
        this.#isConsoleClosed = false;

        while(this.#device.readable && !this.#isConsoleClosed) {
            this.#reader = this.#device.readable.getReader();

            try {
                while(true) {
                    const { value, done } = await this.#reader.read();
                    if (done) {
                      // reader.cancel() has been called.
                      break;
                    }
                    
                    onData(decoder.decode(value));
                }
            } catch(e) {
                console.log('Non vol', e);
            } finally {
                this.#reader.releaseLock();
                this.#reader = undefined;
            }
        }
    
        /*while (true && !this.#isConsoleClosed) {
            const val = await this.#transport.rawRead();
            if (val) {
                onData(decoder.decode(val));
            } else {
                break;
            }
        }*/
    }

    async stopReading() {
        this.#isConsoleClosed = true;

        await serialDevice.releaseReaders();
    }

    get isReading() {
        return !this.#isConsoleClosed;
    }

    #deviceDisconnect() {
        console.log('DISCONNECT');
        this.#device = undefined;
        this.#transport = undefined;
        this.#espLoader = undefined;

        this.#isConsoleClosed = true;

        this.#onDisconnect();
    }
}
