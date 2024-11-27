import { Component, Inject, OnDestroy } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { MatCardModule } from '@angular/material/card';
import { MAT_DIALOG_DATA, MatDialogModule, MatDialogRef } from '@angular/material/dialog';
import { MatExpansionModule } from '@angular/material/expansion';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { BTConnectionAbortedError, BTConnectionFactoryService, BTConnectionGATTConnectionError, BTConnectionPrimaryServiceError, BTConnectionVersionMissmatchError, OTACommand, PSPBluetooth } from '../services/btconnection-factory.service';
import { ILogger } from '../ILogger';
import { firstValueFrom } from 'rxjs';
import { GithubService, ReleaseType } from '../github.service';
import { MatProgressBarModule } from '@angular/material/progress-bar';

enum OTAProgrammerState {
  WarningPrompt,
  ProgramPrompt,
  ProgrammingActive,
  ProgrammingSuccess,
  ProgrammingError
}

export interface IOTAProgrammerDialogData {
  binaryUrl: string,
  assetId: string,
  tag: string
}

@Component({
  selector: 'app-otaprogrammer',
  standalone: true,
  imports: [MatProgressBarModule, MatDialogModule, MatButtonModule, MatCardModule, MatExpansionModule, MatIconModule, MatFormFieldModule, MatInputModule],
  templateUrl: './otaprogrammer.component.html',
  styleUrl: './otaprogrammer.component.scss'
})
export class OTAProgrammerComponent implements OnDestroy {
  constructor(
    public dialogRef: MatDialogRef<OTAProgrammerComponent>,
    @Inject(MAT_DIALOG_DATA) public data: IOTAProgrammerDialogData,
    public btConnectionFactory: BTConnectionFactoryService,
    private gihubService: GithubService
  ) { }

  programProgress = 0;

  get progressBarMode() {
    return this.state === OTAProgrammerState.ProgrammingActive ? 'buffer' : 'determinate';
  }

  ngOnDestroy(): void {
    this.connection?.disconnect();
  }

  logger: ILogger = {
    clean: () => {
      this.terminalOutput = '';
    },
    writeLine: (value: string) => {
      this.terminalOutput += `\n${value}`
    },
    write: (value: string) => {
      this.terminalOutput += value;
    },
    prettyPrint: (value: Record<string, any>) => {
      this.terminalOutput += JSON.stringify(value, null, 2);
    }
  }

  connection?: PSPBluetooth;

  terminalOutput = '';

  newVersion = '';
  oldVersion = '';

  state = OTAProgrammerState.WarningPrompt;
  programmingStates = OTAProgrammerState;

  get closeDisabled() {
    return this.dialogRef.disableClose;
  };

  /*get disableNext() {
    return this.state === OTAProgrammerState.ProgrammingActive ||
      this.state === OTAProgrammerState.ApplyUpdatePrompt ||
      this.state === OTAProgrammerState.ProgrammingSuccess;
  }*/

      disableNext = false;

  nextText = 'Next';

  close () {
    if (!this.dialogRef.disableClose)
      this.dialogRef.close();
  }

  reconnectDisabled = true;

  async next () {
    switch(this.state) {
      case OTAProgrammerState.WarningPrompt:
        this.state = this.programmingStates.ProgramPrompt;
      break;
      case OTAProgrammerState.ProgramPrompt:
      case OTAProgrammerState.ProgrammingError:
        this.state = OTAProgrammerState.ProgrammingActive;
        this.#program();
      break;
    }
  }

  async #program() {
    try {
      this.programProgress = 0;
      this.state = OTAProgrammerState.ProgrammingActive;

      this.dialogRef.disableClose = true;
      this.disableNext = true;

      this.connection = await this.btConnectionFactory.connect(this.logger);
      this.logger.writeLine('Connected to the BLE Server');

      this.oldVersion = await this.connection?.version();

      this.logger.writeLine(`Current Version: ${this.oldVersion}`);

      this.logger.writeLine('Downloading binary...');
      // throw some delays in to make it look busy
      await new Promise(res => setTimeout(res, 2000));

      const binaryData = await firstValueFrom(this.gihubService.getReleaseBinary(this.data.tag, ReleaseType.OTA));

      this.logger.writeLine(`Downloaded, size: ${binaryData.size}`);

      const otaSize = binaryData.size;
      let sent = 0;

      await this.connection?.sendOTACommand(OTACommand.startUpload);

      this.logger.writeLine('Uploading the binary, this can take a while');

      while (sent < otaSize) {
        const remaining = otaSize - sent;
        const toTake = remaining > 244 ? 244 : remaining; 
        const dataToSend = new DataView(binaryData.buffer, sent, toTake);
  
        await this.connection?.sendOTAData(dataToSend);
  
        sent += toTake;

        this.programProgress = Math.round((sent / otaSize) * 100);
      }
      
      this.logger.writeLine('Update has been uploaded. Instructing the PSP Bluetooth Mod to reboot and apply the update. Please wait...');

      
    this.connection?.sendOTACommand(OTACommand.applyUpdate)
      .catch((e) => {
        console.log('Apply error (this is expected)', e);
      });

    await new Promise(res => setTimeout(res, 1000));

    this.connection.disconnect();

    await new Promise(res => setTimeout(res, 5000));

    await this.connection.reconnect();

    this.newVersion = await this.connection?.version() || '';
    
    this.state = OTAProgrammerState.ProgrammingSuccess;

    this.dialogRef.disableClose = false;

    } catch(e) {
      this.state = OTAProgrammerState.ProgrammingError;
      if (e instanceof BTConnectionVersionMissmatchError) {
        this.logger.clean();
        this.logger.writeLine(
          `The device you conencted to does not appear to be the PSP Bluetooth Mod.
When requesting version number it did not match the value expected.

Recieved: ${e.recieved}
Expected: ${e.expected}

Click \'Next\' to try again`
        );

      } else if (e instanceof BTConnectionPrimaryServiceError) {
        this.logger.clean();
        this.logger.writeLine(
          `The device you conencted to does not appear to be the PSP Bluetooth Mod.
It does not have the expected Primary Service.

Click \'Next\' to try again`
        );
      } else if (e instanceof BTConnectionAbortedError) {
        this.logger.clean();
        this.logger.writeLine('Bluetooth device was not selected. Click \'Next\' to try again');
      } else if (e instanceof BTConnectionGATTConnectionError) {
        this.logger.clean();
        this.logger.writeLine(
          `We couldnt connect to your device, tried three times.
Remember Web Bluetooth is an experimental technology and can be unreliable.
It may be best to reload your browser and try again`
        );
      } else {
        this.logger.writeLine('Error during OTA update:');
  
        if (e instanceof Error) {          
          this.logger.writeLine(e?.message);
          this.logger.writeLine(e?.stack || '' );
        }
      }

      this.dialogRef.disableClose = false;
      this.disableNext = false;
    }
  }
}
