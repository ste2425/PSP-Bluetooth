import { Component, Inject } from '@angular/core';
import { MAT_DIALOG_DATA, MatDialogRef, MatDialogModule } from '@angular/material/dialog';
import {MatButtonModule} from '@angular/material/button';
import {MatIconModule} from '@angular/material/icon';
import {MatCardModule} from '@angular/material/card';
import { EspService, ProgramOptions } from '../esp.service';
import { GithubService } from '../github.service';
import {MatExpansionModule} from '@angular/material/expansion';
import {MatFormFieldModule} from '@angular/material/form-field';
import {MatInputModule} from '@angular/material/input';
import { firstValueFrom } from 'rxjs';

export interface IProgrammerDialogData
{
  binaryUrl: string,
  assetId: string,
  tag: string
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
    private esp: EspService,
    private gihubService: GithubService
  ) {}

  programming = false;
  terminalOutput = '';

  confirmed = false;

  terminal = {
    clean: () => this.terminalOutput = '',
    writeLine: (value: string) => this.terminalOutput += `${value}\n`,
    write: (value: string) => this.terminalOutput += value
  }

  disableNext() {
    return this.programming;
  }

  next() {
    if (!this.confirmed) {
      this.confirmed = true;
      this.program();
    }
  }

  async close() {
    if (this.esp.isConnected) {
      this.terminal.writeLine('Disconnecting from ESP, please wait...');

      this.programming = true;
  
      await this.esp.disconnect();

      // give enough time to make it look like were busy
      await new Promise(r => setTimeout(r, 3000));
    }

    this.dialogRef.close();
  }

  async program() {
    try {
      this.programming = true;
      this.terminal.writeLine('Downloading binary...');

      const binaryData = await firstValueFrom(this.gihubService.getReleaseBinary(this.data.tag));

      this.terminal.writeLine(`Downloaded, size: ${binaryData.size}`);

      this.terminal.writeLine('Getting connection to ESP...');

      if (!this.esp.isConnected) {
        await this.esp.setup(() => console.log('DISCONNECT'));
  
        const chip = await this.esp.connect({
          baudrate: 115200,
          //romBaudrate: 115200,
          terminal: this.terminal
        } as any);
  
        this.terminal.writeLine(`Connected: ${chip}`);
      } else {
        this.terminal.writeLine('Already connected to device, moving on.');
      }

      this.terminal.writeLine('Programming...');

      const flashOptions: ProgramOptions = {
        fileArray: [{
            address: 0,
            data: binaryData.data
        }],
        flashSize: "keep",
        eraseAll: false,
        compress: true,
        reportProgress: (percentageComplete: number) => {
          console.log('PROGRESS', percentageComplete);
        },
        flashMode: 'keep',
        flashFreq: 'keep'
    };

      await this.esp.program(flashOptions);

      await this.esp.disconnect();

      this.terminal.writeLine('Complete.');
    } catch (e) {
      console.log('ERROR', e);

      if (e instanceof Error)
        this.terminal.writeLine(e.toString());
    } finally {
      this.programming = false;
    }
  }

}
