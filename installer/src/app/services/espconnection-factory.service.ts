import { Injectable } from '@angular/core';
import { SerialConnectionFactoryService, SerialPortConnection } from './serial-connection-factory.service';
import { ESPLoader, FlashOptions, LoaderOptions, Transport } from 'esptool-js';

const DISCONNECT_EXCEPTION_MESSAGE = 'Failed to execute \'close\' on \'SerialPort\': The port is already closed.';

export class ESPSerialConnection {
  #transport?: Transport;
  #decoder = new TextDecoder();
  #connected = false;

  constructor(private serialPort: SerialPortConnection) { }

  get connected() {
    return this.#connected;
  }

  async connect(baud = 115200) {
    if (!this.serialPort.serialPort)
      return;

    this.#transport = new Transport(this.serialPort.serialPort, true);

    await this.#transport.connect(baud);

    this.#connected = true;
  }

  async startReading(onData: (val: string) => void) {
    while (true && this.#connected) {
      const val = await this.#transport?.rawRead();

      if (val)
        onData(this.#decoder.decode(val));
      else
        break;
    }
  }

  async disconnect(clean: boolean = false) {
    try {      
      if (clean)
        sessionStorage.removeItem('deviceinfo');
      
      await this.serialPort.releaseReaders();
      await this.serialPort.releaseWriters();
  
      await this.#transport?.disconnect();
      await this.#transport?.waitForUnlock(1500);
  
      this.#connected = false;
    } catch(e) {
      const isDiconnectError = e instanceof DOMException && e.message === DISCONNECT_EXCEPTION_MESSAGE;

      if (!isDiconnectError)
        throw e;
    }
  }
}

export class ESPProgrammingConnection {
  #espLoader?: ESPLoader;
  #transport?: Transport;
  #connected = false;

  constructor(private serialPort: SerialPortConnection) { }

  get connected() {
    return this.#connected;
  }

  async connect(opts: LoaderOptions) {
    if (!this.serialPort.serialPort)
      throw new Error('SerialPort undefined');

    if (this.#connected)
      return await this.#espLoader?.main();

    this.#transport = new Transport(this.serialPort.serialPort, true);
    this.#espLoader = new ESPLoader({
      ...opts,
      transport: this.#transport
    });

    const chip = await this.#espLoader.main();

    this.#connected = true;

    return chip;
  }

  async program(options: FlashOptions) {
    if (!this.#espLoader)
      throw new Error('ESP loader not instantiated, call connect first');

    await this.#espLoader.writeFlash(options);
  }

  async disconnect(clean: boolean = false) {
    try {
      if (clean)
        sessionStorage.removeItem('deviceinfo');

      await this.serialPort.releaseReaders();
      await this.serialPort.releaseWriters();
  
      await this.#transport?.disconnect();
      await this.#transport?.waitForUnlock(1500);
  
      this.#connected = false;
    } catch(e) {
      const isDiconnectError = e instanceof DOMException && e.message === DISCONNECT_EXCEPTION_MESSAGE;

      if (!isDiconnectError)
        throw e;
    }
  }
}

@Injectable({
  providedIn: 'root'
})
export class ESPConnectionFactoryService {

  constructor(private serialConnectionService: SerialConnectionFactoryService) { }

  async serialConnection() {
    const port = await this.serialConnectionService.connect();

    return new ESPSerialConnection(port);
  }

  async programmingConnection() {
    const port = await this.serialConnectionService.connect();

    return new ESPProgrammingConnection(port);
  }
}
