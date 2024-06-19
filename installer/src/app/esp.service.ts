import { Injectable } from '@angular/core';
import { SerialService } from './serial.service';
import { ESPLoader, FlashOptions, LoaderOptions, Transport } from 'esptool-js';

export type ProgramOptions = Omit<FlashOptions, 'reportProgress' | 'calculateMD5Hash'> & { reportProgress: (percentage: number) => void }

@Injectable({
  providedIn: 'root'
})
export class EspService {
  #device: SerialPort | undefined;
  #transport: Transport | undefined;
  #espLoader: ESPLoader | undefined;
  #isConnected = false;
  #isConsoleClosed = true;
  #onDisconnect: (() => void) | undefined;
  #reader: ReadableStreamDefaultReader<any> | undefined;
  #decoder = new TextDecoder();

  get isConnected() {
    return this.#isConnected;
  }

  constructor(private serialDevice: SerialService) { }

  async resetDevice() { }

  async setup(onDisconnect: () => void) {
    this.#onDisconnect = onDisconnect;

    this.#device = await this.serialDevice.connect();

    if (!this.#device)
      return;

    this.#device?.addEventListener('disconnect', this.#deviceDisconnect);
    this.#transport = new Transport(this.#device, false);
  }

  async connect(options: Omit<LoaderOptions, 'transport'>): Promise<string | void> {
    if (!this.#transport)
      return;

    const flashOptions: LoaderOptions = {
      transport: this.#transport,
      ...options
    };
    this.#espLoader = new ESPLoader(flashOptions);

    const model = await this.#espLoader.main();

    this.#isConnected = true;

    return model;
  }

  async disconnect() {
    await this.serialDevice.releaseReaders();

    if (this.#transport)
      this.#transport.disconnect();

    this.#deviceDisconnect();
  }

  async program(options: ProgramOptions) {
    if (!this.#espLoader)
      return;

    const flashOptions: FlashOptions = {
      ...options,
      reportProgress: (fileIndex, written, total) => {
        options.reportProgress((written / total) * 100);
      }
     // calculateMD5Hash: (image) => CryptoJS.MD5(CryptoJS.enc.Latin1.parse(image))
    };

    await this.#espLoader.writeFlash(flashOptions);
  }

  async beginReading(onData: (value: string) => void) {
    this.#isConsoleClosed = false;

    while (this.#device?.readable && !this.#isConsoleClosed) {
      this.#reader = this.#device.readable.getReader();

      try {
        while (true) {
          const { value, done } = await this.#reader.read();
          if (done) {
            // reader.cancel() has been called.
            break;
          }

          onData(this.#decoder.decode(value));
        }
      } catch (e) {
        console.log('Non vol', e);
      } finally {
        this.#reader.releaseLock();
        this.#reader = undefined;
      }
    }
  }

  async stopReading() { 
    this.#isConsoleClosed = true;

    await this.serialDevice.releaseReaders();
  }

  get isReading() {
    return !this.#isConnected;
  }

  #deviceDisconnect() {
    console.log('DISCONNECT');
    this.#device = undefined;
    this.#transport = undefined;
    this.#espLoader = undefined;

    this.#isConsoleClosed = true;
    this.#isConnected = false;

    if (this.#onDisconnect)
      this.#onDisconnect();
  }
}
