import { Component, inject, OnDestroy, OnInit } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { BTConnectionAbortedError, BTConnectionFactoryService, BTConnectionGATTConnectionError, BTConnectionPrimaryServiceError, BTConnectionVersionMissmatchError, IControllerMapping, OTACommand, PSPBluetooth } from '../services/btconnection-factory.service';
import { MatDialog } from '@angular/material/dialog';
import { AddMappingModalComponent } from '../add-mapping-modal/add-mapping-modal.component';
import { TextFieldModule } from '@angular/cdk/text-field';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';
import { ControllerMapingComponent } from "../controller-maping/controller-maping.component";
import { MatProgressSpinner, MatSpinner } from '@angular/material/progress-spinner';
import {MatIconModule} from '@angular/material/icon';
import { Title } from '@angular/platform-browser';

interface IViewControllerMapping extends IControllerMapping {
  visible?: boolean
}
@Component({
  selector: 'app-configuration-page',
  standalone: true,
  imports: [MatIconModule, MatProgressSpinner, MatButtonModule, TextFieldModule, FormsModule, CommonModule, ControllerMapingComponent],
  templateUrl: './configuration-page.component.html',
  styleUrl: './configuration-page.component.scss'
})
export class ConfigurationPageComponent implements OnDestroy, OnInit {
  ngOnInit(): void {
    this.titleService.setTitle('Button Config');
  }
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
  titleService = inject(Title);
  terminalOutput = '';

  configurations: IViewControllerMapping[] = []

  addMapping() {
    const dialogRef = this.dialogService.open(AddMappingModalComponent);

    dialogRef.afterClosed().subscribe(result => {
      console.log('The dialog was closed', result);
    });
  }

  next() {
    for (const configuration of this.configurations) {
      const index = this.configurations.indexOf(configuration),
        isLast = this.configurations.length - 1 === index;

      if (configuration.visible) {
        configuration.visible = false;

        this.configurations[isLast ? 0 : index + 1 ].visible = true;
        break;
      }
    }
  }

  back() {
    for (const configuration of this.configurations) {
      const index = this.configurations.indexOf(configuration),
        isFirst = index === 0;

      if (configuration.visible) {
        configuration.visible = false;

        this.configurations[isFirst ? this.configurations.length - 1 : index - 1 ].visible = true;
        break;
      }
    }
  }

  async loadMappings() {
    try {
      this.loading = true;
      this.deviceNotSelectedError = false;
      this.wrongBoardError = false;
      this.connectionError = false;
      this.generalError = '';
      this.loadMappingsDisabled = true;

      if (!this.btDevice)
        await this.connect();
  
      if (!this.btDevice)
        throw new Error('Uh oh');
  
      console.log('Reading mappings');
      const mappings = await this.btDevice.loadButtonMappings();
  
      if (mappings) {
        this.configurations = mappings;

        this.configurations[0].visible = true;

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
