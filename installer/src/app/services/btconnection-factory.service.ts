import { Injectable } from '@angular/core';
import { ILogger } from '../ILogger';
import { availableControllerBitsMappedToType, availablecontrollerTypes, availablePspButtons } from './ESPValueDefinitions';

const PRIMARY_SERVICE_UUID = '4627c4a4-ac00-46b9-b688-afc5c1bf7f63';
const VERSION_UUID = '4627c4a4-ac01-46b9-b688-afc5c1bf7f63';
const MAPPINGS_UUID = '4627c4a4-ac03-46b9-b688-afc5c1bf7f63';
const EXPECTED_VERSION_PREFIX = 'PSP Bluetooth Version';
const OTA_COMMANDS_UUID = '4627c4a4-ac04-46b9-b688-afc5c1bf7f63';
const OTA_DATA_UUID = '4627c4a4-ac05-46b9-b688-afc5c1bf7f63';

export enum OTACommand {
    startUpload = 0,
    uploadFinished = 1,
    applyUpdate = 2
}

export type ButtonMapping = [number, number, number];

export interface IControllerMapping {
    m: ButtonMapping[],
    n: number,
    c: [number, number, number, number],
    cHex: string
}
  
export class PSPBluetooth {
    constructor(private device: BluetoothDevice, private primaryService: BluetoothRemoteGATTService) { }

    get connected() {
        return !!this.device.gatt?.connected;
    }

    deviceDetails() {
        return {
            id: this.device.id,
            name: this.device.name
        };
    }

    async listCharacteristics() {
        if (!this.primaryService)
            throw new Error('Primary service not connected');

        return await this.primaryService.getCharacteristics();
    }

    version() {
        return this.#readValue(VERSION_UUID);
    }

    async loadButtonMappings(): Promise<IControllerMapping[]> {
        const strValue = await this.#readValue(MAPPINGS_UUID);
        
        if (!strValue)
            return JSON.parse("[{\"n\":1,\"c\":[255,0,0,0.1],\"m\":[[8,0,0],[10,1,0],[9,3,0],[0,2,0],[15,0,1],[17,1,1],[16,2,1],[2,0,2],[1,1,2],[4,3,2],[6,2,2],[5,5,2],[7,6,2],[0,1,3]]},{\"n\":2,\"c\":[255,0,0,1],\"m\":[[8,0,0],[10,1,0],[9,3,0],[0,2,0],[15,0,1],[17,1,1],[16,2,1],[2,0,2],[1,1,2],[4,3,2],[6,2,2],[5,5,2],[7,6,2],[102,6,2],[101,7,2]]}]");

        const data = JSON.parse(strValue) as IControllerMapping[];

        if (!Array.isArray(data))
            return [];

        return data.map((dataRaw) => {
            return {
                m: this.#validateControllerMappings(dataRaw.m),
                n: this.#validateMappingControllerBumber(dataRaw.n),
                c: this.#validateMappingColour(dataRaw.c),
                get cHex() {
                    const toHex = (c: number) => {
                        var hex = c.toString(16);
                        return hex.length == 1 ? "0" + hex : hex;
                    };
                    
                    return `#${toHex(this.c[0])}${toHex(this.c[1])}${toHex(this.c[2])}`;
                },
                set cHex(value: string) {                    
                    const r = parseInt(value.substring(1,2), 16)
                    const g = parseInt(value.substring(3,2), 16)
                    const b = parseInt(value.substring(5,2), 16)

                    this.c[0] = r;
                    this.c[1] = g;
                    this.c[2] = b;
                } 
            }
        })
    }

    #validateControllerMappings(potentialMappings: any): [number, number, number][] {
        if (!potentialMappings || !Array.isArray(potentialMappings))
            return [];

        return potentialMappings.filter((m) => {
            //only keep a mapping if all its values are valid.
            if (!Array.isArray(m) || m.length !== 3)
                return false

            const [pspButton, controllerBit, controllerType] = m;

            if (!availablePspButtons.includes(pspButton)) {
                console.error('Mapping PSP Button invalid: ', pspButton);
                return false;
            }
            if (!availablecontrollerTypes.includes(controllerType)) {
                console.error('Mapping Controller Type invalid: ', controllerType);
                return false;
            }
            if (!availableControllerBitsMappedToType[controllerType].includes(controllerBit)) {
                console.error('Mapping controller bits for type invalid: ', controllerType, controllerBit);
                return false;
            }

            return true;
        });
    } 

    #validateMappingControllerBumber(potentialNumber: any): number {
        const number = parseInt(potentialNumber);

        return isNaN(number) ? 1 : number;
    }

    #validateMappingColour(potentialColour: any): [number, number, number, number] {
        if (!potentialColour || !Array.isArray(potentialColour))
            return [255, 0, 0, 1];

        const r = parseInt(potentialColour[0]),
            g = parseInt(potentialColour[1]),
            b = parseInt(potentialColour[2]),
            brightness = parseInt(potentialColour[3]);

        return [
            isNaN(r) ? 255 : r,
            isNaN(g) ? 0 : g,
            isNaN(b) ? 0 : b,
            isNaN(brightness) ? 1 : brightness,
        ];
    }

    async saveButtonMappings(mappings: IControllerMapping[]) {
        const mappingsJson = JSON.stringify(mappings);
        
        await this.#writeValue(MAPPINGS_UUID, this.#encode(mappingsJson));
    }

    connectToCharacteristic(uuid: string) {
        return this.primaryService?.getCharacteristic(uuid);
    }

    async sendOTAData(data: ArrayBuffer | DataView) {

        await this.#writeValue(OTA_DATA_UUID, data);
    }

    async sendOTACommand(command: OTACommand) {
        const value = new Uint8Array([command]);

        await this.#writeValue(OTA_COMMANDS_UUID, value);
    }

    async #readValue(uuid: string) {
        if (!this.device.gatt?.connected)
            throw new Error('Unable to read value. GATT not connected');

        const characteristic = await this.primaryService.getCharacteristic(uuid);
        const rawValue = await characteristic.readValue();

        return this.#decode(rawValue);
    }

    async #writeValue(uuid: string, value: BufferSource) {
        if (!this.device.gatt?.connect)
            throw new Error('Unable to read value. GATT not connected');

        const characteristic = await this.primaryService.getCharacteristic(uuid);
        await characteristic.writeValueWithResponse(value);
    }

    #encode(value: string) {
        const encoder = new TextEncoder();
        return encoder.encode(value);
    }

    #decode(value: DataView) {
      const decoder = new TextDecoder();
      return decoder.decode(value).replaceAll('\0', '');
    }

    disconnect() {
        if (this.connected)
            this.device.gatt?.disconnect();
    }

    async reconnect() {
        await this.device?.gatt?.connect();

        // when re-connecting we need to re-request the primary service
        const getPrimaryService = async () => {
            const primaryService = await this.device.gatt?.getPrimaryService(PRIMARY_SERVICE_UUID);
            
            if (!primaryService)
                throw new BTConnectionPrimaryServiceError('Unable to find primary service');
    
            this.primaryService = primaryService;
        }

        try {
            await getPrimaryService();
        } catch(e) {
            if (e instanceof Error && e.name === 'NetworkError') {
                await this.device.gatt?.connect(); 
                
                await getPrimaryService();    
            } else {
                throw e;
            }
        }
    }
}

export class BTError extends Error {}
export class BTConnectionGATTConnectionError extends BTError {}
export class BTConnectionAbortedError extends BTError {}
export class BTConnectionUnknownError extends BTError {}
export class BTConnectionVersionMissmatchError extends BTError {
    constructor(public recieved: string, public expected: string) {
        super();
    }
}
export class BTConnectionPrimaryServiceError extends BTError {}

@Injectable({
    providedIn: 'root'
})
export class BTConnectionFactoryService {
    async retryOnError<T>(handler: () => Promise<T>): Promise<T> {
        let runCount = 1

        const doIt: () => Promise<T> = async () => {
            try {
                return await handler();
            } catch (e) {
                console.log(e);

                if (runCount > 3)
                    throw e;

                runCount++;

                await new Promise(res => setTimeout(res, 1000));

                return await doIt();
            }
        }

        return await doIt();
    }

    isSupported() {
        return 'bluetooth' in navigator;
    }

    async connect(logger: ILogger): Promise<PSPBluetooth> {
        try {        
            logger.writeLine('Requesting access to device ...');

            const device = await navigator.bluetooth.requestDevice({
                acceptAllDevices: true,
                optionalServices: [PRIMARY_SERVICE_UUID]
            });
    
            /**
             * There is odd behaviour where we connect fine here, but by the time we ask for the primary service we have disconnected.
             * Even though `device.gatt.connected` reports true.
             * 
             * So we have a retry on the initial connect.
             * 
             * Then we have a retry on the get primary service, on each error first trying to re-connect.
             * Bit crazy but it seems to work.
             */
            await this.retryOnError( async () => {
                logger.writeLine('Attempting to connect');
    
                await device.gatt?.connect();
            });
    
            return await this.retryOnError<PSPBluetooth>(async () => {
                try {
                    const primaryService = await device.gatt?.getPrimaryService(PRIMARY_SERVICE_UUID);
            
                    if (!primaryService)
                        throw new BTConnectionPrimaryServiceError('Unable to find primary service');
            
                    const pspBT = new PSPBluetooth(device, primaryService);
            
                    const version = await pspBT.version();
        
                    if (!version.startsWith(EXPECTED_VERSION_PREFIX))
                        throw new BTConnectionVersionMissmatchError(version, EXPECTED_VERSION_PREFIX || 'lol');
            
                    return pspBT;
                } catch (e) {
                    // Network Errors are reported when GATT server is disconnected.
                    // If that is the error Attempt to re-connect then re-throw original error
                    // So retry wrapper can re-run 
                    if (e instanceof Error && e.name === 'NetworkError') {
                        await device.gatt?.connect();                
                    // handle error where connected device does not have requested primary service    
                    } else if (e instanceof Error && e.message.includes('No Services matching'))
                        throw new BTConnectionPrimaryServiceError('Unable to find primary service');
    
                    throw e;
                }
            }).catch(async e => {
                // If we cant connect for any reason disconnect from the device
                if (device)
                    device.gatt?.disconnect();

                throw e;
            });
        } catch(e) {
            // Handle errors to use custom classes for specific situations.
            if (!(e instanceof Error)) {
                console.log(e);                
                throw new BTConnectionUnknownError();
            } else if (e instanceof BTError) {
                throw e;
            }

            switch(e.name) {
                case 'NotFoundError':
                    throw new BTConnectionAbortedError(e.message);
                case 'NetworkError':
                    throw new BTConnectionGATTConnectionError(e.message);
                default:
                    throw new BTConnectionUnknownError();
            }
        }
    }
}
