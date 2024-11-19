import { Injectable } from '@angular/core';
import { ILogger } from '../ILogger';

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

    async loadButtonMappings(): Promise<Record<string, any> | undefined> {
        const strValue = await this.#readValue(MAPPINGS_UUID);
        
        if (!strValue)
            return undefined;

        return JSON.parse(strValue);
    }

    async saveButtonMappings(mappings: Record<string, any>) {
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
        if (!this.device.gatt?.connect)
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
}

@Injectable({
    providedIn: 'root'
})
export class BTConnectionFactoryService {
    async connect(logger: ILogger): Promise<PSPBluetooth> {
        logger.writeLine('Connecting to device...');

        const device = await navigator.bluetooth.requestDevice({
            acceptAllDevices: true,
            optionalServices: [PRIMARY_SERVICE_UUID]
        });

        //logger.writeLine('Connected to device.');

        //logger.writeLine('Connecting to GATT Service...');

        await device.gatt?.connect();

        //logger.writeLine('Connected to GATT Service');

        //logger.writeLine('Connecting to primary service...');

        const primaryService = await device.gatt?.getPrimaryService(PRIMARY_SERVICE_UUID);

        if (!primaryService)
            throw new Error('Unable to find primary service');

        logger.writeLine('Got Primary Service');

        const pspBT = new PSPBluetooth(device, primaryService);

        const version = await pspBT.version();

        if (!version.startsWith(EXPECTED_VERSION_PREFIX))
            throw new Error(`Version Characteristic did not start with expected data: '${version}'`);

        return pspBT;
    }
}
