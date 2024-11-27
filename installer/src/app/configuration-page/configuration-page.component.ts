import { Component, inject, OnDestroy } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { BTConnectionAbortedError, BTConnectionFactoryService, BTConnectionGATTConnectionError, BTConnectionPrimaryServiceError, BTConnectionVersionMissmatchError, IControllerMapping, OTACommand, PSPBluetooth } from '../services/btconnection-factory.service';
import { MatDialog } from '@angular/material/dialog';
import { AddMappingModalComponent } from '../add-mapping-modal/add-mapping-modal.component';
import { TextFieldModule } from '@angular/cdk/text-field';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';
import { ControllerMapingComponent } from "../controller-maping/controller-maping.component";
import { MatProgressSpinner, MatSpinner } from '@angular/material/progress-spinner';

@Component({
  selector: 'app-configuration-page',
  standalone: true,
  imports: [MatProgressSpinner, MatButtonModule, TextFieldModule, FormsModule, CommonModule, ControllerMapingComponent],
  templateUrl: './configuration-page.component.html',
  styleUrl: './configuration-page.component.scss'
})
export class ConfigurationPageComponent implements OnDestroy {
  loadMappingsDisabled = false;

  btDevice?: PSPBluetooth;
  wrongBoardError = false;
  deviceNotSelectedError = false;
  connectionError = false;
  generalError = '';
  loading = false;

  get connected() {
    return this.btDevice?.connected;
  }

  ngOnDestroy(): void {
    this.btDevice?.disconnect();
  }

  btConnectionFactory = inject(BTConnectionFactoryService);
  dialogService = inject(MatDialog);
  terminalOutput = '';

  configurations: IControllerMapping[] = []

  addMapping() {
    const dialogRef = this.dialogService.open(AddMappingModalComponent);

    dialogRef.afterClosed().subscribe(result => {
      console.log('The dialog was closed', result);
    });
  }

  async loadMappings() {
    try {
      this.loading = true;
      this.deviceNotSelectedError = false;
      this.wrongBoardError = false;
      this.connectionError = false;
      this.generalError = '';
      this.loadMappingsDisabled = true;
      await this.connect();
  
      if (!this.btDevice)
        throw new Error('Uh oh');
  
      console.log('Reading mappings');
      const mappings = await this.btDevice.loadButtonMappings();
  
      if (mappings) {
        this.configurations = mappings;
        //this.$configurations.next(mappings);
        console.log('Mappings read');
      } else {
        console.log('no mappings');
      }
  
      this.loadMappingsDisabled = false;
    } catch(e) {
      console.log(e);
      this.loadMappingsDisabled = false;
    } finally {
      this.loading = false;
    }
  }

  async saveValue() {
    if (!this.connected || !this.btDevice)
      return;

    console.log('Saving mappings...');

    //await this.btDevice.saveButtonMappings(JSON.parse(this.configuration));
    
    console.log("Mappings written");
  }

  async connect() {  
      try {      
        console.log('Connecting...');
        
        this.btDevice = await this.btConnectionFactory.connect({
          write(value) {
            this.writeLine(value);
          },
          writeLine(value) {
            console.log(value);
          },
          clean() {},
          prettyPrint(value) {
            console.log(value);
          }
        });

        console.log('Connected');
        console.log(this.btDevice.deviceDetails());

        console.log('Connected');

        const version = await this.btDevice.version();
        console.log(version);
      } catch(e) {if (e instanceof BTConnectionVersionMissmatchError) {
        this.wrongBoardError = true;
        
        console.log(
          `The device you conencted to does not appear to be the PSP Bluetooth Mod.
When requesting version number it did not match the value expected.

Recieved: ${e.recieved}
Expected: ${e.expected}`
        );

      } else if (e instanceof BTConnectionPrimaryServiceError) {
        this.wrongBoardError = true;
        console.log(
          `The device you conencted to does not appear to be the PSP Bluetooth Mod.
It does not have the expected Primary Service.`
        );
      } else if (e instanceof BTConnectionAbortedError) {
        
        console.log('Bluetooth device was not selected. Click \'Next\' to try again');
        this.deviceNotSelectedError = true;
      } else if (e instanceof BTConnectionGATTConnectionError) {
        this.connectionError = true;
        console.log(
          `We couldnt connect to your device, tried three times.
Remember Web Bluetooth is an experimental technology and can be unreliable.
It may be best to reload your browser and try again`
        );
      } else {
        console.log('Error during OTA update:');
  
        if (e instanceof Error) {    
          this.generalError = `
${e.message};

--------

${e.stack}`;   
          console.log(e?.message);
          console.log(e?.stack || '' );
        }
      }

      }

  }
}
