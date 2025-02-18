import { Component, inject, OnDestroy } from '@angular/core';
import { PanelComponent } from '../../panel/panel.component';
import { ISettings, SettingsService, SettingsServiceFactory } from '../../bt/settings.service';
import { BTConnectionFactoryService } from '../../bt/btconnection-factory.service';
import { BTConnectionAbortedError, BTConnectionGATTConnectionError, BTConnectionPrimaryServiceError, BTConnectionVersionMissmatchError } from '../../bt/errors';
import { FormsModule } from '@angular/forms';
import {MatInputModule} from '@angular/material/input';
import {MatFormFieldModule} from '@angular/material/form-field';
import { MatCheckboxModule } from '@angular/material/checkbox';
import { MatSliderModule } from '@angular/material/slider';
import { MatButtonModule } from '@angular/material/button';
import { MatSelectModule } from '@angular/material/select';

@Component({
  selector: 'app-settings-page',
  standalone: true,
  imports: [MatSelectModule, MatButtonModule, MatSliderModule, MatCheckboxModule, PanelComponent, FormsModule, MatInputModule, MatFormFieldModule],
  templateUrl: './settings-page.component.html',
  styleUrl: './settings-page.component.scss'
})
export class SettingsPageComponent implements OnDestroy {
  version = '';
  loading = false;
  saving = false;
  connected = false;
  firmwareTooLow = false;
  settingsService?: SettingsService;

  MIN_FIRMWARE_VERSION = '2.3.0';

  settingsFactory = inject(SettingsServiceFactory);
  btConnectionFactory = inject(BTConnectionFactoryService);

  settings?: ISettings;

  screenDelay = 0;
  coldBootDelay = 0;
  warmBootDelay = 0;
  shutdownDelay = 0;

  ngOnDestroy(): void {
    if (this.settingsService)
      this.settingsService.disconnect();
  }

  async loadConfig() {
    try {
      this.loading = true;

      if (!this.settingsService)
        await this.#connect();

      this.settings = await this.settingsService?.readSettings();

      if (!this.settings)
        return;

      this.screenDelay = this.settings.screenDelay / 1000;
      this.coldBootDelay = this.settings.screenDelay / 1000;
      this.warmBootDelay = this.settings.screenDelay / 1000;
      this.shutdownDelay = this.settings.shutdownDelay / 1000;
    } catch (e) {
      this.connected = false;
    } finally {
      this.loading = false;
    }
  }

  async saveConfig() {
    if (!this.settings)
      return;

    this.saving = true;

    try {
      await this.settingsService?.saveSettings({
        ...this.settings,
        screenDelay: Math.round(this.screenDelay * 1000),
        warmBootDelay: Math.round(this.warmBootDelay * 1000),
        coldBootDelay: Math.round(this.coldBootDelay * 1000),
        shutdownDelay: Math.round(this.shutdownDelay * 1000)
      });
    } catch(e) {
      console.error('Error saving', e);
    }

    this.saving = false;
  }

  async #connect() {
    try {
      console.log('Connecting...');

      const connection = await this.btConnectionFactory.connect({
        write(value) {
          this.writeLine(value);
        },
        writeLine(value) {
          console.log(value);
        },
        clean() { },
        prettyPrint(value) {
          console.log(value);
        }
      });

      this.connected = true;

      this.version = await connection.version();
      this.firmwareTooLow = connection.versionTooLow(this.version, this.MIN_FIRMWARE_VERSION)

      if (!this.firmwareTooLow)
        this.settingsService = this.settingsFactory.create(connection);
    } catch (e) {
      if (e instanceof BTConnectionVersionMissmatchError) {
        console.log(
          `The device you conencted to does not appear to be the PSP Bluetooth Mod.
  When requesting version number it did not match the value expected.
  
  Recieved: ${e.recieved}
  Expected: ${e.expected}`
        );

      } else if (e instanceof BTConnectionPrimaryServiceError) {
        console.log(
          `The device you conencted to does not appear to be the PSP Bluetooth Mod.
  It does not have the expected Primary Service.`
        );
      } else if (e instanceof BTConnectionAbortedError) {

        console.log('Bluetooth device was not selected. Click \'Next\' to try again');
      } else if (e instanceof BTConnectionGATTConnectionError) {
        console.log(
          `We couldnt connect to your device, tried three times.
  Remember Web Bluetooth is an experimental technology and can be unreliable.
  It may be best to reload your browser and try again`
        );
      } else {
        console.log('Error during OTA update:');

        if (e instanceof Error) {
          /*this.generalError = `
  ${e.message};
  
  --------
  
  ${e.stack}`;*/
          console.log(e?.message);
          console.log(e?.stack || '');
        }
      }

    }

  }
}
