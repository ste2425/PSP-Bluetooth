import { Component } from '@angular/core';
import {MatButtonModule} from '@angular/material/button';
import { ESPConnectionFactoryService, ESPSerialConnection } from '../services/espconnection-factory.service';

@Component({
  selector: 'app-serial-page',
  standalone: true,
  imports: [MatButtonModule],
  templateUrl: './serial-page.component.html',
  styleUrl: './serial-page.component.scss'
})
export class SerialPageComponent {
  constructor(private espFactory: ESPConnectionFactoryService) {}

  terminalOutput = '';

  get conncted () {
    return !!this.#connection?.connected;
  }

  #connection: ESPSerialConnection | undefined;

  terminal = {
    clean: () => this.terminalOutput = '',
    writeLine: (value: string) => this.terminalOutput += `${value}\n`,
    write: (value: string) => this.terminalOutput += value
  }

  async connect() {
    try {
      this.#connection = await this.espFactory.serialConnection();
  
      await this.#connection.connect();
  
      this.terminal.writeLine('Connected.');
  
      await this.#connection.startReading((v) => this.terminal.write(v));

      this.terminal.writeLine('Finished reading.');
    } catch(e) {
      console.error(e);

      if (e instanceof Error)
        this.terminal.writeLine(e.toString());
    }
  }

  async disconnect() {
    if (!this.#connection)
      return;

    try {
      await this.#connection.disconnect();
    } catch(e) {
      console.error(e);

      if (e instanceof Error)
        this.terminal.writeLine(e.toString());
    }
  }

  ngOnDestroy() {
    this.disconnect();
  }
}
