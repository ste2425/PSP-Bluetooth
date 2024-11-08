import { Component, inject } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { BTConnectionFactoryService, OTACommand, PSPBluetooth } from '../services/btconnection-factory.service';
import { MatDialog } from '@angular/material/dialog';
import { AddMappingModalComponent } from '../add-mapping-modal/add-mapping-modal.component';
import { TextFieldModule } from '@angular/cdk/text-field';
import { FormsModule } from '@angular/forms';
import { ILogger } from '../ILogger';

@Component({
  selector: 'app-configuration-page',
  standalone: true,
  imports: [MatButtonModule, TextFieldModule, FormsModule],
  templateUrl: './configuration-page.component.html',
  styleUrl: './configuration-page.component.scss'
})
export class ConfigurationPageComponent {
  btConnectionFactory = inject(BTConnectionFactoryService);
  dialogService = inject(MatDialog);
  terminalOutput = '';

  configuration: string = '';

  btDevice?: PSPBluetooth;

  get connected() {
    return !!this.btDevice?.connected;
  }

  OTACommand = OTACommand;

  async otaCommand(command: OTACommand) {
    await this.btDevice?.sendOTACommand(command);
  }

  terminal: ILogger = {
    clean: () => this.terminalOutput = '',
    writeLine: (value: string) => this.terminalOutput += `${value}\n`,
    write: (value: string) => this.terminalOutput += value,
    prettyPrint (value: Record<string, any>) {
      this.writeLine(JSON.stringify(value, null, 2))
    }
  }

  addMapping() {
    const dialogRef = this.dialogService.open(AddMappingModalComponent);

    dialogRef.afterClosed().subscribe(result => {
      console.log('The dialog was closed', result);
    });
  }

  decode(value: DataView) {
    const decoder = new TextDecoder();
    return decoder.decode(value).replaceAll('\0', '');
  }

  async readValue() {
    if (!this.connected || !this.btDevice)
      return;

    this.terminal.writeLine('Reading mappings');
    const mappings = await this.btDevice.loadButtonMappings();

    if (mappings) {
      this.configuration = JSON.stringify(mappings);
      this.terminal.writeLine('Mappings read');
    } else {
      this.terminal.writeLine('no mappings');
    }
  }

  async saveValue() {
    if (!this.connected || !this.btDevice)
      return;

    this.terminal.writeLine('Saving mappings...');

    await this.btDevice.saveButtonMappings(JSON.parse(this.configuration));
    
    this.terminal.writeLine("Mappings written");
  }

  async connect() {
    
    try {      
      this.terminal.writeLine('Connecting...');
  
      this.btDevice = await this.btConnectionFactory.connect(this.terminal);

      this.terminal.writeLine('Connected');
      this.terminal.prettyPrint(this.btDevice.deviceDetails());

      this.terminal.writeLine('Connected');

      const version = await this.btDevice.version();
      this.terminal.writeLine(version);
    } catch(e) {
      console.error(e);

      if (e instanceof Error)
        this.terminal.writeLine(e.toString());

    }
  }

  async disconnect() {
  }
}