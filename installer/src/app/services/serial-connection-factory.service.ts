import { Injectable } from '@angular/core';

export class SerialPortConnection {
  #proxiedSerialPort?: SerialPort;
  #readers: ReadableStreamReader<any>[] = [];
  #writers: any[] = [];

  constructor(serialPort: SerialPort) {
    this.#proxiedSerialPort = new Proxy(serialPort, this.#proxyHandler<SerialPort>((target, name) => {
      if (name == 'readable' && target.readable)
        return this.#proxyReadableStream(target.readable);

      if (name === 'writable' && target.writable)
        return this.#proxyWriteableStream(target.writable);
    }));
  }

  get serialPort() {
    return this.#proxiedSerialPort;
  }

  async releaseReaders() {
    if (!this.#readers.length)
      return;

    this.#readers.forEach(r => r.releaseLock());
    this.#readers.length = 0;

    // Probably not needed, but we give it a little time to release
    await new Promise(r => setTimeout(r, 500));
  }

  async releaseWriters() {
    if (!this.#writers.length)
      return;

    this.#writers.forEach(r => r.releaseLock());
    this.#writers.length = 0;

    // Probably not needed, but we give it a little time to release
    await new Promise(r => setTimeout(r, 500));
  }

  // We need to proxy the reader too so we know when it was closed
  // This allows us to remove it from our list of watched readers
  // Some code paths in the esptool lib do correctly close the readers, this is for them
  #proxyReader(reader: ReadableStreamReader<any>) {
    return new Proxy(reader, this.#proxyHandler((target, name) => {
      if (name === 'releaseLock' || name === 'close') {
        const index = this.#readers.indexOf(reader);

        if (index != -1) {
          this.#readers.splice(index, 1);
        }
      }
    }));
  }

  #proxyWriter(writer: WritableStreamDefaultWriter) {
    return new Proxy(writer, this.#proxyHandler((target, name) => {
      if (name === 'releaseLock' || name === 'close') {
        const index = this.#writers.indexOf(writer);

        if (index != -1) {
          this.#writers.splice(index, 1);
        }
      }
    }));
  }

  #wrappedGetReaderFn(_target: ReadableStream<any>) {
    const wrappedThis = this;

    return function (...args: any[]) {
      const reader = _target.getReader(...args);

      wrappedThis.#readers.push(reader);

      return wrappedThis.#proxyReader(reader);
    }.bind(_target);
  }

  #wrappedGetWriter(_target: WritableStream<any>) {
    const wrappedThis = this;

    return function () {
      const writer = _target.getWriter();

      wrappedThis.#writers.push(writer);

      return wrappedThis.#proxyWriter(writer);
    }.bind(_target);
  }

  #proxyReadableStream(readableStream: ReadableStream<any>) {
    return new Proxy(readableStream, this.#proxyHandler<ReadableStream<any>>((_target, name) => {
      if (name === 'getReader')
        return this.#wrappedGetReaderFn(readableStream);

      return undefined;
    }));
  }

  #proxyWriteableStream(writeableStream: WritableStream<any>) {
    return new Proxy(writeableStream, this.#proxyHandler<WritableStream<any>>((_target, name) => {
      if (name === 'getWriter')
        return this.#wrappedGetWriter(writeableStream);

      return undefined;
    }));
  }

  /** General utility to save duplicated code */
  #proxyHandler<T>(extra: (target: T, name: keyof T) => any | undefined = () => { }) {
    return {
      get(target: any, name: any) {
        const resp = extra(target, name);

        if (resp)
          return resp;

        if (typeof target[name] === "function") {
          return target[name].bind(target);
        }

        return target[name];
      }
    }
  }
}

@Injectable({
  providedIn: 'root'
})
export class SerialConnectionFactoryService {

  async connect() {
    const ports = await navigator.serial.getPorts();

    let device = ports.find(p => JSON.stringify(p.getInfo()) === sessionStorage.getItem('deviceinfo'));

    if (!device) {
      console.log('Device not already connected');

      device = await navigator.serial.requestPort();

      const deviceInfo = JSON.stringify(device.getInfo());

      if (deviceInfo !== '{}')
        sessionStorage.setItem('deviceinfo', deviceInfo);
    }

    return new SerialPortConnection(device);
  }

  isSupported() {
    return 'serial' in navigator;
  }
}
