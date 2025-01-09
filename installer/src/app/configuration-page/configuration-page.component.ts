import { Component, inject, OnDestroy } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { TextFieldModule } from '@angular/cdk/text-field';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';
import { ControllerMapingComponent } from "../controller-maping/controller-maping.component";
import { MatProgressSpinner } from '@angular/material/progress-spinner';
import {MatIconModule} from '@angular/material/icon';
import { ColourPickerComponent } from "../colour-picker/colour-picker.component";
import { TourMatMenuModule, TourService } from 'ngx-ui-tour-md-menu';
import { DialogService } from '../dialogs/dialog.service';
import { PanelComponent } from '../panel/panel.component';
import { controllerTypes, dpadControllerBits, pspButtons } from '../services/ESPValueDefinitions';
import { BTConnectionFactoryService } from '../bt/btconnection-factory.service';
import { ConfigurationService, ConfigurationServiceFactory, IControllerMapping, MAX_GROUPINGS } from '../bt/configuration.service';
import { BTConnectionAbortedError, BTConnectionGATTConnectionError, BTConnectionPrimaryServiceError, BTConnectionVersionMissmatchError } from '../bt/errors';
import semver from 'semver';
import { MatSelectModule } from '@angular/material/select';

interface IViewControllerMapping extends IControllerMapping {
  visible?: boolean
}
@Component({
  selector: 'app-configuration-page',
  standalone: true,
  imports: [MatSelectModule, PanelComponent, TourMatMenuModule, MatIconModule, MatProgressSpinner, MatButtonModule, TextFieldModule, FormsModule, CommonModule, ControllerMapingComponent, ColourPickerComponent],
  templateUrl: './configuration-page.component.html',
  styleUrl: './configuration-page.component.scss'
})
export class ConfigurationPageComponent implements OnDestroy {
  dialogService = inject(DialogService);
  btConnectionFactory = inject(BTConnectionFactoryService);
  configFactory = inject(ConfigurationServiceFactory);
  readonly tourService = inject(TourService);

  loadMappingsDisabled = false;
  MIN_FIRMWARE_VERSION = '2.0.0';

  version = '';

  wrongBoardError = false;
  deviceNotSelectedError = false;
  connectionError = false;
  generalError = '';
  loading = false;
  firmwareTooLow = false;

  configService?: ConfigurationService;

  get connected() {
    return !!this.configService?.connected();
  }

  get addDisabled() {
    return this.configurations.length >= MAX_GROUPINGS;
  }

  deleteMapping(mapping: IViewControllerMapping) {
    const index = this.configurations.indexOf(mapping);

    if (index !== undefined && index !== null && index > -1) {
      this.configurations.splice(index, 1);
    }

    this.configurations.forEach(c => c.visible = false);

    if (this.configurations.length === 0) {
      this.visibleItem = undefined;

      return;
    }

    const visibleitem = index < this.configurations.length ?
      this.configurations[index] :
      this.configurations[this.configurations.length - 1];

    visibleitem.visible = true;

    this.visibleItem = visibleitem;
  }

  async resetSettings() {
    if (this.configService) {
      if (await this.dialogService.confirmResetConfig()) {
        await this.configService.resetToDefaults();
        await this.loadMappings();
      }
    }
  }

  addNewControllerMapping() {
    if (this.configurations.length >= MAX_GROUPINGS)
      return;
    
    const newConfig: IViewControllerMapping = {
      visible: true,
      n: 1,
      c: [255, 200, 10, 1],
      m: [
        [pspButtons.left, dpadControllerBits.dpadLeft, controllerTypes.dpad]
      ]
    };

    this.configurations.forEach(c => c.visible = false);

    this.configurations.push(newConfig);

    this.visibleItem = newConfig;
  }

  onColourChange(mapping: IControllerMapping, event: { r: number, g: number, b: number, a: number }) {
    mapping.c[0] = event.r;
    mapping.c[1] = event.g;
    mapping.c[2] = event.b;
    mapping.c[3] = event.a;
  }

  ngOnDestroy(): void {
    this.configService?.disconnect();
  }

  terminalOutput = '';

  configurations: IViewControllerMapping[] = [];// JSON.parse("[{\"visible\": true,\"n\":1,\"c\":[255,0,0,0.1],\"m\":[[8,0,0],[10,1,0],[9,3,0],[0,2,0],[15,0,1],[17,1,1],[16,2,1],[2,0,2],[1,1,2],[4,3,2],[6,2,2],[5,5,2],[7,6,2],[0,1,3]]},{\"n\":2,\"c\":[255,0,0,1],\"m\":[[8,0,0],[10,1,0],[9,3,0],[0,2,0],[15,0,1],[17,1,1],[16,2,1],[2,0,2],[1,1,2],[4,3,2],[6,2,2],[5,5,2],[7,6,2],[102,6,2],[101,7,2]]}]");

  visibleItem?: IViewControllerMapping = this.configurations[0];
  next() {
    for (const configuration of this.configurations) {
      const index = this.configurations.indexOf(configuration),
        isLast = this.configurations.length - 1 === index;

      if (configuration.visible) {
        configuration.visible = false;

        const next = this.configurations[isLast ? 0 : index + 1 ];
        
        next.visible = true;

        this.visibleItem = next;
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

        const next = this.configurations[isFirst ? this.configurations.length - 1 : index - 1 ];

        next.visible = true;

        this.visibleItem = next;

        break;
      }
    }
  }

  mappingsSaving = false;

  async saveMappings() {
    if (!this.configService)
      return;

    this.mappingsSaving = true;
    this.loading = true;

    try {
      this.configurations = this.configurations.filter(x => x.m.length);
      
      const toSave = this.configurations.map((x, i) => {
        const y = {
          c: x.c,
          n: x.n,
          m: x.m
        };

        if (y.m.length)
          return y;
  
        return undefined;
      }).filter((x: IControllerMapping | undefined): x is IControllerMapping => !!x);

      console.log('toSave', toSave)

      await this.configService.saveButtonMappings(toSave);

      await this.loadMappings();

      this.dialogService.configSaved();
    } catch(e) {
      console.log(e);
    } finally {      
      this.mappingsSaving = false;
      this.loading = false;
    }
  }

  startTour() {
    if (!this.configurations.length)
      this.addNewControllerMapping();
    
    if (this.visibleItem && !this.visibleItem.m.length)
      this.visibleItem.m.push([pspButtons.left, dpadControllerBits.dpadLeft, controllerTypes.dpad])

    this.tourService.start();
  }

  async loadMappings() {

    try {
      this.loading = true;
      this.deviceNotSelectedError = false;
      this.wrongBoardError = false;
      this.connectionError = false;
      this.generalError = '';
      this.loadMappingsDisabled = true;

      if (!this.configService)
        await this.connect();

      if (this.firmwareTooLow) {
        this.loading = false;
        this.loadMappingsDisabled = true;
        return;
      }

  
      if (!this.configService)
        throw new Error('No Config Service');

      const mappings = await this.configService.loadButtonMappings();
  
      if (mappings?.length) {
        this.configurations = mappings;

        this.configurations[0].visible = true;

        this.visibleItem = this.configurations[0];
      }
  
      this.loadMappingsDisabled = false;

      if (!localStorage.getItem('config.tourPrompted')) {        
        localStorage.setItem('config.tourPrompted', 'true');
        const doTour = await this.dialogService.confirmConfigTour();

        if (doTour)
          this.startTour();
      }
    } catch(e) {
      console.log(e);
      this.loadMappingsDisabled = false;
    } finally {
      this.loading = false;
    }
  }

  async connect() {  
      try {      
        console.log('Connecting...');
        
        const connection = await this.btConnectionFactory.connect({
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

        this.version = await connection.version();
        this.firmwareTooLow = this.version.endsWith('<DEVELOPMENT>') ? false : semver.lt(this.version.replace(/[^\d.]/g,''), this.MIN_FIRMWARE_VERSION);
        
        if (!this.firmwareTooLow)
          this.configService = this.configFactory.create(connection);
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
