import { Injectable } from '@angular/core';
import { BTConnection } from './btConnection';
import { SETTINGS_UUID } from './uuids';
import { BTCommands } from './btCommands';

const DEFAULT_SETTINGS: ISettings = {
    ledBrightness: 100,
    throttleThreshold: 370,
    analogThreshold: 300,
    diagonalThreshold: 330,
    useExtraCombo: true,
    screenDelay: 7000,
    coldBootDelay: 13000,
    warmBootDelay: 5000,
    useAlternativeShutdown: false,
    shutdownDelay: 3000,
    batteryWarning: 38, //15% of 255
    clickMode: 1, // controller pair
    lngClickMode: 2, // BLE mode
    dblClickMode: 0 // nothing
};

export interface ISettings {
    ledBrightness: number,
    throttleThreshold: number,
    analogThreshold: number,
    diagonalThreshold: number,
    useExtraCombo: boolean,
    screenDelay: number,
    coldBootDelay: number,
    warmBootDelay: number,
    shutdownDelay: number,
    useAlternativeShutdown: boolean,
    batteryWarning: number,
    clickMode: number,
    dblClickMode: number,
    lngClickMode: number
}

export class SettingsService {
    constructor(private conn: BTConnection) {}

    disconnect() {
        this.conn.disconnect();
    }

    connected() {
        return this.conn.connected;
    }

    async saveSettings(settings: ISettings) {
        const settingsJson = JSON.stringify(settings);

        await this.conn.sendCommand(BTCommands.settingsUploadStart);

        const chunks = this.#chunk(settingsJson, 100);

        for (const chunk of chunks) {
            await this.conn.writeValue(SETTINGS_UUID, this.conn.encode(chunk));
        }

        const strValue = await this.#readSettings();

        await this.conn.sendCommand(BTCommands.settingsUploadComplete);

        if (strValue.trim() !== settingsJson.trim())
            throw new Error(`
Unable to apply settings, uploaded settings data does not match what was expected.
Uploaded: 
${strValue.trim()}
Expected:
${settingsJson.trim()}
`);
        
        await this.conn.sendCommand(BTCommands.settingsUploadApply);
    }

    async #readSettings(): Promise<string> {  
        await this.conn.sendCommand(BTCommands.settingsReadStart);

        let moreData = true,
            configString = '',
            itteration = 0;

        while (moreData && itteration < 25) {
            const strValue = await this.conn.readValue(SETTINGS_UUID),
                byteSize = new Blob([strValue]).size;

            configString += strValue;
            itteration++;

            if (byteSize < 100)
                moreData = false;            
        }

        return configString;
    }

    async readSettings(): Promise<ISettings> {  
        const configString = await this.#readSettings();
        
        if (!configString)
            return {...DEFAULT_SETTINGS};

        let data: ISettings = {...DEFAULT_SETTINGS};

        try {
            data = { ...data, ...JSON.parse(configString)};
        } catch(e) {
            console.log('Error parsing JSON', e, configString);
        }

        return data;
    }

    #chunk(s: string, maxBytes: number): string[] {
        const data = [];
        const decoder = new TextDecoder("utf-8");
        let buf = new TextEncoder().encode(s);
        
        while (buf.length) {
            const chunk = buf.slice(0, maxBytes);
            let chunkStr = decoder.decode(chunk);
            
            if (chunk.byteLength < maxBytes) {
                const spaces = maxBytes - chunk.byteLength,
                    padding = ' '.repeat(spaces);

                chunkStr += padding;

                console.log(new Blob([chunkStr]).size);
            }

            data.push(chunkStr);
            buf = buf.slice(maxBytes);
        }
        
        return data;
    }
}

@Injectable({
    providedIn: 'root'
})
export class SettingsServiceFactory {
    create(conn: BTConnection) {
        return new SettingsService(conn);
    }
}
