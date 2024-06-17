/*
    There is a bug in the latest release of esptool-js which seems to keep readers open.
    This means when attempting to disconenct a device it will sit and wait for the reader to release its lock.

    This never happens.

    The latest main has this bug fixed, however that has not made it into an NPM release yet.

    So we proxy the SerialPort device to track the number of readers created.

    This allows us to manually release any that still exist (should only ever be one at a time)
    when we wish to disconnect. 

    The downside is we have to proxy a number of layers to get down to the actual reader.
*/

const readers = [];

export async function connect() {
    const ports = await navigator.serial.getPorts();
    
    let device = ports.find(p => JSON.stringify(p.getInfo()) === sessionStorage.getItem('deviceinfo'));

    if (!device) {
        console.log('Not already found');
        device = await navigator.serial.requestPort({});
    
        const info = device.getInfo();
    
        sessionStorage.setItem('deviceinfo', JSON.stringify(info));
    }

    return new Proxy(device, proxyHandler((target, name) => {
        if (name == 'readable')
            return proxyReadableStream(target.readable);
    }));
}

export async function releaseReaders() {
    if (!readers.length)
        return;

    readers.forEach(r => r.releaseLock());
    readers.length = 0;

    // Probably not needed, but we give it a little time to release
    await new Promise(r => setTimeout(r, 500));
}

export default {
    connect,
    releaseReaders
}

// We need to proxy the reader too so we know when it was closed
// This allows us to remove it from our list of watched readers
// Some code paths in the esptool lib do correctly close the readers, this is for them
function proxyReader(reader) {
    return new Proxy(reader, proxyHandler((target, name) => {
        if (name === 'releaseLock' || name === 'close') {
            const index = readers.indexOf(reader);

            if (index != -1) {
                readers.splice(index, 1);
            }
        }
    }));
}

function wrappedGetReaderFn(_target) {
    return function (...args) {
        const reader = _target.getReader(...args)

        readers.push(reader);

        return proxyReader(reader);
    }.bind(_target);
}

function proxyReadableStream(readableStream) {
    return new Proxy(readableStream, proxyHandler((target, name) => {
        if (name === 'getReader')
            return wrappedGetReaderFn(readableStream);
    }));
}

/** General utility to save duplicated code */
function proxyHandler(extra = () => { }) {
    return {
        get(target, name) {
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