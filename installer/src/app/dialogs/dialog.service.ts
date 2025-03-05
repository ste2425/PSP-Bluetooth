import { inject, Injectable } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';
import { ConfirmationModalComponent, IConfirmationData } from './confirmation-modal/confirmation-modal.component';
import { firstValueFrom } from 'rxjs';
import { IOKModalData, OKModalComponent } from './ok-modal/ok-modal.component';

@Injectable({
  providedIn: 'root'
})
export class DialogService {
  dialogService = inject(MatDialog);

  async ShowMissingBLEButtonMode() {
    const data: IConfirmationData = {
      title: 'Invalid Button Modes',
      message: 'At least one button mode must toggle BLE mode. Otherwise you will be locked out of the web app.',
      hideNoButton: true
    };

    await firstValueFrom(
      this.dialogService.open(ConfirmationModalComponent, {
        data
      }).afterClosed()
    );
  }

  async confirmConfigTour(): Promise<boolean> {
    const data: IConfirmationData = {
      title: 'Take a short tour?',
      message: 'Looks like you\'re new here. Fancy a quick tour?'
    };

    const result = await firstValueFrom(
      this.dialogService.open(ConfirmationModalComponent, {
        data
      }).afterClosed()
    )

    return !!result;
  }

  async confirmResetConfig(): Promise<boolean> {
    const data: IConfirmationData = {
      title: 'Reset Button Mappings?',
      message: 'This will remove all custom buttons mappings and reset to defaults. Continue?'
    };

    const result = await firstValueFrom(
      this.dialogService.open(ConfirmationModalComponent, {
        data
      }).afterClosed()
    )

    return !!result;
  }

  async configSaved() {
    const data: IOKModalData = {
      title: 'Data Saved',
      message: 'Configuration data has been saved'
    };

    await firstValueFrom(
      this.dialogService.open(OKModalComponent, {
        data
      }).afterClosed()
    )
  }
}
