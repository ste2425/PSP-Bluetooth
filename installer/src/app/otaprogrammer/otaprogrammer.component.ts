import { Component, Inject, OnDestroy } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { MatCardModule } from '@angular/material/card';
import { MAT_DIALOG_DATA, MatDialogModule, MatDialogRef } from '@angular/material/dialog';
import { MatExpansionModule } from '@angular/material/expansion';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { BTConnectionFactoryService, OTACommand, PSPBluetooth } from '../services/btconnection-factory.service';
import { ILogger } from '../ILogger';
import { firstValueFrom } from 'rxjs';
import { GithubService, ReleaseType } from '../github.service';
import { MatProgressBarModule } from '@angular/material/progress-bar';

enum OTAProgrammerState {
  WarningPrompt,
  ProgramPrompt,
  ApplyUpdatePrompt,
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

  get disableNext() {
    return this.state === OTAProgrammerState.ProgrammingActive ||
      this.state === OTAProgrammerState.ApplyUpdatePrompt ||
      this.state === OTAProgrammerState.ProgrammingSuccess;
  }

  nextText = 'Next';

  close () {
    if (!this.dialogRef.disableClose)
      this.dialogRef.close();
  }

  reconnectDisabled = true;

  async reConnect() {    
    try {
      this.connection = await this.btConnectionFactory.connect(this.logger);

      this.newVersion = await this.connection?.version() || '';
  
      this.state = OTAProgrammerState.ProgrammingSuccess;
      this.dialogRef.disableClose = false;
    } catch(e) {
      if (e instanceof Error)
        this.logger.writeLine(e.message);
    }
  }

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
      case OTAProgrammerState.ProgrammingActive:
        this.state = OTAProgrammerState.ApplyUpdatePrompt;
      break;
    }
  }

  async #applyUpdate() {
    if (this.state !== OTAProgrammerState.ProgrammingActive)
      return;

    this.state = OTAProgrammerState.ApplyUpdatePrompt;

    this.connection?.sendOTACommand(OTACommand.applyUpdate)
      .catch((e) => {
        console.log('Apply error (this is expected)', e);
      });

    await new Promise(res => setTimeout(res, 10000));

    this.reconnectDisabled = false;
  }

  async #program() {
    try {
      this.programProgress = 0;
      this.state = OTAProgrammerState.ProgrammingActive;

      this.dialogRef.disableClose = true;

      this.connection = await this.btConnectionFactory.connect(this.logger);
      this.logger.writeLine('Connected to the BLE Server');

      this.oldVersion = await this.connection?.version();

      this.logger.writeLine(`Current Version: ${this.oldVersion}`);

      this.logger.writeLine('Downloading binary');
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
      
      this.#applyUpdate();
    } catch(e) {
      this.logger.writeLine('Error during OTA update:');

      if (e instanceof Error)
        this.logger.writeLine(e?.message);

      this.state = OTAProgrammerState.ProgrammingError;
    }
  }
}
