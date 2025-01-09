import { Injectable } from '@angular/core';
import { ILogger } from '../ILogger';
import { BTConnection } from './btConnection';
import { EXPECTED_VERSION_PREFIX, PRIMARY_SERVICE_UUID } from './uuids';
import { BTConnectionAbortedError, BTConnectionGATTConnectionError, BTConnectionPrimaryServiceError, BTConnectionUnknownError, BTConnectionVersionMissmatchError, BTError } from './errors';

@Injectable({
    providedIn: 'root'
})
export class BTConnectionFactoryService {

    private lastConnection?: BTConnection;

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

    connected() {
        return !!this.lastConnection?.device?.gatt?.connected;
    }

    disconnect() {
        this.lastConnection?.device?.gatt?.disconnect();
    }

    async connect(logger: ILogger): Promise<BTConnection> {
        try {        
            logger.writeLine('Requesting access to device ...');

            let device: BluetoothDevice;

            if (this.lastConnection) {
                device = this.lastConnection?.device;
            } else {
                device = await navigator.bluetooth.requestDevice({
                    acceptAllDevices: true,
                    optionalServices: [PRIMARY_SERVICE_UUID]
                });
            }            

            if (!device) 
                throw new BTConnectionUnknownError("Device is undefined");
    
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
    
            return await this.retryOnError<BTConnection>(async () => {
                try {
                    const primaryService = await device.gatt?.getPrimaryService(PRIMARY_SERVICE_UUID);
            
                    if (!primaryService)
                        throw new BTConnectionPrimaryServiceError('Unable to find primary service');
            
                    const pspBT = new BTConnection(device, primaryService);
            
                    const version = await pspBT.version();
        
                    if (!version.startsWith(EXPECTED_VERSION_PREFIX))
                        throw new BTConnectionVersionMissmatchError(version, EXPECTED_VERSION_PREFIX);
                    
                    this.lastConnection = pspBT;

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
