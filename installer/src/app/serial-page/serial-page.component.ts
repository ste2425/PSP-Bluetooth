import { Component } from '@angular/core';
import { EspService } from '../esp.service';
import {MatButtonModule} from '@angular/material/button';

@Component({
  selector: 'app-serial-page',
  standalone: true,
  imports: [MatButtonModule],
  templateUrl: './serial-page.component.html',
  styleUrl: './serial-page.component.scss'
})
export class SerialPageComponent {
  constructor(private espService: EspService) {}

  terminalOutput = '';

  get conncted () {
    return this.espService.isConnected;
  }

  terminal = {
    clean: () => this.terminalOutput = '',
    writeLine: (value: string) => this.terminalOutput += `${value}\n`,
    write: (value: string) => this.terminalOutput += value
  }

  async disconnect() {
    await this.espService.stopReading();

    this.terminal.writeLine('Stop');

    await this.espService.disconnect();

    this.terminal.writeLine('Disonnected');
  }

  async connect() {
    try {
      await this.espService.setup(() => this.terminal.writeLine('Disconnected'));

      const chip = await this.espService.connect({
        baudrate: 115200,
        //romBaudrate: 115200,
        terminal: this.terminal
      } as any);

      this.terminal.writeLine(`Connected, chip: ${chip}`);

      await this.espService.beginReading((d) => this.terminal.writeLine(d));

      this.terminal.writeLine('Connect Resolved');
    } catch(e) {
      console.error(e);

      if (e instanceof Error)
        this.terminal.writeLine(e.toString());
    }
  }
}
