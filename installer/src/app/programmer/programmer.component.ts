import { Component, Inject } from '@angular/core';
import { MAT_DIALOG_DATA, MatDialogRef, MatDialogModule } from '@angular/material/dialog';
import {MatButtonModule} from '@angular/material/button';
import {MatCardModule} from '@angular/material/card';
import { EspService } from '../esp.service';
import { GithubService } from '../github.service';

export interface IProgrammerDialogData
{
  binaryUrl: string,
  assetId: string
}

@Component({
  selector: 'app-programmer',
  standalone: true,
  imports: [MatDialogModule, MatButtonModule, MatCardModule],
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

  terminal = {
    clean: () => this.terminalOutput = '',
    writeLine: (value: string) => this.terminalOutput += `${value}\n`,
    write: (value: string) => this.terminalOutput += value
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

      const binaryString = await this.gihubService.downloadBinary(this.data.assetId);
      this.terminal.writeLine(binaryString);

      this.terminal.writeLine('Downloaded.');

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
    } catch (e) {
      console.log('ERROR', e);

      if (e instanceof Error)
        this.terminal.writeLine(e.toString());
    } finally {
      this.programming = false;
    }
  }

}
