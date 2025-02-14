import { BTCommands } from "./btCommands";
import { COMMANDS_UUID, VERSION_UUID } from "./uuids";
import semver from 'semver';

export class BTConnection {	
    constructor(public device: BluetoothDevice, private primaryService: BluetoothRemoteGATTService) { }

	get connected() {
		return !!this.device?.gatt?.connected;
	}

    deviceDetails() {
        return {
            id: this.device.id,
            name: this.device.name
        };
    }

	version(): Promise<string> {
        return this.readValue(VERSION_UUID);
	}

    versionTooLow(versionToheck: string, minVersion: string) {
        const version = versionToheck.match(/(\d+\.\d+\.\d?)/g)?.[0] || ''
        return versionToheck.endsWith('<DEVELOPMENT>') ? false : semver.lt(version, minVersion, { includePrerelease: true } as any);
    }

	async sendCommand(command: BTCommands) {
        const value = new Uint8Array([command]);

        await this.writeValue(COMMANDS_UUID, value);
	}

	async readValue(uuid: string) {		
        if (!this.device.gatt?.connected)
            throw new Error('Unable to read value. GATT not connected');

        const characteristic = await this.primaryService.getCharacteristic(uuid);
        const rawValue = await characteristic.readValue();
        
        return this.#decode(rawValue);
	}

    async writeValue(uuid: string, value: BufferSource) {
        if (!this.device.gatt?.connect)
            throw new Error('Unable to read value. GATT not connected');

        const characteristic = await this.primaryService.getCharacteristic(uuid);
        await characteristic.writeValueWithResponse(value);
    }

    encode(value: string) {
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