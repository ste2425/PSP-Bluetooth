import { Injectable } from '@angular/core';
import { BTCommands } from './btCommands';
import { IReleaseBinary } from '../github.service';
import { OTA_DATA_UUID } from './uuids';
import { BTConnection } from './btConnection';
import { ILogger } from '../ILogger';

export class OTAService {
    constructor(private conn: BTConnection) {}

    async* performUpdate(updateBinary: IReleaseBinary, logger: ILogger) {
        logger.writeLine('Starting procces');

        await this.conn.sendCommand(BTCommands.OTAStart);

        const otaSize = updateBinary.size;
        let sent = 0;

        while (sent < otaSize) {
            const remaining = otaSize - sent;
            const toTake = remaining > 244 ? 244 : remaining; 
            const dataToSend = new DataView(updateBinary.buffer, sent, toTake);
      
            await this.conn?.writeValue(OTA_DATA_UUID, dataToSend);
      
            sent += toTake;
    
            yield Math.round((sent / otaSize) * 100);
        }

        logger.writeLine("Fully uploaded, Applying update");

        // Don't await this as it will error when the ESP disconnects during reset.
        this.conn.sendCommand(BTCommands.OTAApply);

        logger.writeLine("Waiting for disconnect");
        await new Promise(res => setTimeout(res, 1000));

        logger.writeLine("Disconnecting");
        this.conn.disconnect();
    }
}

@Injectable({
    providedIn: 'root'
})
export class OTAServiceFactory {
    create(conn: BTConnection) {
        return new OTAService(conn);
    }
}
