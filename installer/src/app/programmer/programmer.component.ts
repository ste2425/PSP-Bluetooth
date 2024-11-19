import { Component, HostListener, Inject } from '@angular/core';
import { MAT_DIALOG_DATA, MatDialogRef, MatDialogModule } from '@angular/material/dialog';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatCardModule } from '@angular/material/card';
import { GithubService, ReleaseType } from '../github.service';
import { MatExpansionModule } from '@angular/material/expansion';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { firstValueFrom } from 'rxjs';
import { ESPConnectionFactoryService, ESPProgrammingConnection } from '../services/espconnection-factory.service';

export interface IProgrammerDialogData {
  binaryUrl: string,
  assetId: string,
  tag: string
}

enum ProgrammerState {
  WarningPrompt,
  ProgramPrompt,
  ProgrammingActive,
  ProgrammingSuccess,
  ProgrammingError
}

@Component({
  selector: 'app-programmer',
  standalone: true,
  imports: [MatDialogModule, MatButtonModule, MatCardModule, MatExpansionModule, MatIconModule, MatFormFieldModule, MatInputModule],
  templateUrl: './programmer.component.html',
  styleUrl: './programmer.component.scss'
})
export class ProgrammerComponent {
  constructor(
    public dialogRef: MatDialogRef<ProgrammerComponent>,
    @Inject(MAT_DIALOG_DATA) public data: IProgrammerDialogData,
    private espFactory: ESPConnectionFactoryService,
    private gihubService: GithubService
  ) { }

  state = ProgrammerState.WarningPrompt;
  programmingStates = ProgrammerState;

  #espConnection?: ESPProgrammingConnection;

  terminalOutput = '';

  terminal = {
    clean: () => { },
    writeLine: (value: string) => this.terminalOutput += `${value}\n`,
    write: (value: string) => this.terminalOutput += value
  }

  get closeDisabled() {
    return this.state === ProgrammerState.ProgrammingActive;
  }

  async close() {
    await this.disconnect();

    this.dialogRef.close();
  }

  @HostListener('window:beforeunload', ['$event'])
  unloadNotification($event: BeforeUnloadEvent) {
    if (this.state === ProgrammerState.ProgrammingActive) {
      $event.preventDefault();
      $event.returnValue = "Navigating away whilst programming the ESP could lead soft bricking your ESP.";
    }
  }

  get disableNext() {
    return this.state === ProgrammerState.ProgrammingActive || this.state === ProgrammerState.ProgrammingSuccess;
  }

  get nextText() {
    return this.state === ProgrammerState.WarningPrompt ?
      'Next' :
      'Program';
  }

  next() {
    switch(this.state) {
      case ProgrammerState.WarningPrompt:
        this.state = this.programmingStates.ProgramPrompt;
      break;
      case ProgrammerState.ProgramPrompt:
      case ProgrammerState.ProgrammingError:
        this.program();
      break;
    }
  }

  async disconnect(clean: boolean = false) {
    if (this.#espConnection)
      await this.#espConnection.disconnect(clean);

    this.#espConnection = undefined;
  }

  async program() {
    try {
      this.state = ProgrammerState.ProgrammingActive;
      this.dialogRef.disableClose = true;

      if (!this.#espConnection)
        this.#espConnection = await this.espFactory.programmingConnection();

      this.terminal.writeLine('\n\n----\nDo not close your browser as this will abort the flash');
      this.terminal.writeLine('This could lead to your ESP refusing to flash.')
      this.terminal.writeLine('Waiting 10 seconds so you actually read this...\n----\n\n');

      await new Promise((r) => setTimeout(r, 10000));

      this.terminal.writeLine('Downloading binary...');

      const binaryData = await firstValueFrom(this.gihubService.getReleaseBinary(this.data.tag, ReleaseType.Serial));

      this.terminal.writeLine(`Downloaded, size: ${binaryData.size}`);

      this.terminal.writeLine('Getting connection to ESP...');

      if (!this.#espConnection.connected) {
        let timeout: any;

        const timeoutPromise = new Promise((res, rej) => 
          timeout = setTimeout(() => rej(new Error('Connect timeout')), 60000));
        
        const connectPromise = this.#espConnection.connect({
          baudrate: 921600,
          terminal: this.terminal
        } as any);
  
        // if the timeout resolves first it will reject, so chip can only ever be a string
        const chip = await Promise.any([timeoutPromise, connectPromise]) as string;

        clearTimeout(timeout);

        this.terminal.writeLine(`Connected: ${chip}`);
      } else {
        this.terminal.writeLine('Already connected to device, moving on.');
      }

      this.terminal.writeLine('Programming...');

      const flashOptions = {
        fileArray: [{
          address: 0,
          data: binaryData.data
        }],
        flashSize: "keep",
        eraseAll: false,
        compress: true,
        reportProgress: (percentageComplete: number) => {
          console.log('PROGRESS', percentageComplete);
        }
      } as any;

      await this.#espConnection.program(flashOptions);

      this.terminal.writeLine('Complete.');

      await this.disconnect();

      this.terminal.writeLine('Disconnected');

      this.state = ProgrammerState.ProgrammingSuccess;
    } catch (e) {
      console.log('ERROR', e);

      this.state = ProgrammerState.ProgrammingError;

      await this.disconnect(true);

      if (e instanceof Error){
        console.log('Writing to log', e.toString());
        this.terminal.writeLine(`\n${e.toString()}`);
      }
    } finally {
      this.dialogRef.disableClose = false;
    }
  }
}
