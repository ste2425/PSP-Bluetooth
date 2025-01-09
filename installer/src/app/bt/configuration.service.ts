import { Injectable } from '@angular/core';
import { BTConnection } from './btConnection';
import { MAPPINGS_UUID } from './uuids';
import { BTCommands } from './btCommands';
import { analogControllerBits, availableControllerBitsMappedToType, availablecontrollerTypes, availablePspButtons, controllerTypes, pspButtons } from '../services/ESPValueDefinitions';

const DEFAULT_MAPPINGS = "[{\"n\":1,\"c\":[255,0,0,1],\"m\":[[8,0,0],[10,1,0],[9,3,0],[0,2,0],[15,0,1],[17,1,1],[16,2,1],[2,0,2],[1,1,2],[4,3,2],[6,2,2],[5,5,2],[7,6,2],[0,1,3]]},{\"n\":2,\"c\":[255,0,0,1],\"m\":[[8,0,0],[10,1,0],[9,3,0],[0,2,0],[15,0,1],[17,1,1],[16,2,1],[2,0,2],[1,1,2],[4,3,2],[6,2,2],[5,5,2],[7,6,2],[102,6,2],[101,7,2]]}]";

export const MAX_GROUPINGS = 5;
export const MAX_MAPPINGS = 30;

export type ButtonMapping = [number, number, number];

export interface IControllerMapping {
    m: ButtonMapping[],
    n: number,
    c: [number, number, number, number]
}

export class ConfigurationService {
    constructor(private conn: BTConnection) {}

    disconnect() {
        this.conn.disconnect();
    }

    connected() {
        return this.conn.connected;
    }

    async resetToDefaults() {        
        await this.conn.sendCommand(BTCommands.configReset);
    }

    async saveButtonMappings(mappings: IControllerMapping[]) {
        const mappingsJson = JSON.stringify(mappings);

        await this.conn.sendCommand(BTCommands.configUploadStart);

        const chunks = this.#chunk(mappingsJson, 100);

        for (const chunk of chunks) {
            await this.conn.writeValue(MAPPINGS_UUID, this.conn.encode(chunk));
        }

        const strValue = await this.#readConfig();

        await this.conn.sendCommand(BTCommands.configUploadComplete);

        if (strValue.trim() !== mappingsJson.trim())
            throw new Error(`
Unable to apply mappings, saved configuration data does not match what was expected.
Saved: 
${strValue.trim()}
Expected:
${mappingsJson.trim()}
`);
        
        await this.conn.sendCommand(BTCommands.configUploadApply);
    }

    async #readConfig(): Promise<string> {  
        await this.conn.sendCommand(BTCommands.configReadStart);

        let moreData = true,
            configString = '',
            itteration = 0;

        while (moreData && itteration < 25) {
            const strValue = await this.conn.readValue(MAPPINGS_UUID),
                byteSize = new Blob([strValue]).size;

            configString += strValue;
            itteration++;

            if (byteSize < 100)
                moreData = false;            
        }

        return configString;
    }

    async loadButtonMappings(): Promise<IControllerMapping[]> {  
        const configString = await this.#readConfig();
        
        if (!configString || configString.startsWith('[]'))
            return JSON.parse(DEFAULT_MAPPINGS);

        let data: IControllerMapping[] = [];

        try {
            data = JSON.parse(configString);
        } catch(e) {
            console.log('Error parsing JSON', e, configString);
            data = JSON.parse(DEFAULT_MAPPINGS);
        }

        if (!Array.isArray(data))
            return [];

        return data.map((dataRaw) => {
            return {
                m: this.#validateControllerMappings(dataRaw.m),
                n: this.#validateMappingControllerNumber(dataRaw.n),
                c: this.#validateMappingColour(dataRaw.c)
            }
        });
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

            // Dirty. Default data set analog to 0. Detect analog and set to correct value.
            if (controllerType === controllerTypes.analog && 
                (
                    controllerBit === analogControllerBits.leftAnalog || controllerBit === analogControllerBits.rightAnalog
                )
            )
                m[0] = pspButtons.analog;

            return true;
        });
    } 

    #validateMappingControllerNumber(potentialNumber: any): number {
        const number = parseInt(potentialNumber);

        return isNaN(number) ? 1 : number;
    }

    #validateMappingColour(potentialColour: any): [number, number, number, number] {
        if (!potentialColour || !Array.isArray(potentialColour))
            return [255, 0, 0, 1];

        const r = parseInt(potentialColour[0]),
            g = parseInt(potentialColour[1]),
            b = parseInt(potentialColour[2]),
            brightness = parseFloat(potentialColour[3]);

        return [
            isNaN(r) ? 255 : r,
            isNaN(g) ? 0 : g,
            isNaN(b) ? 0 : b,
            isNaN(brightness) ? 1 : brightness,
        ];
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
export class ConfigurationServiceFactory {
    create(conn: BTConnection) {
        return new ConfigurationService(conn);
    }
}
