import { Component, inject } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { MAT_DIALOG_DATA, MatDialogActions, MatDialogContent, MatDialogRef, MatDialogTitle } from '@angular/material/dialog';

export interface IConfirmationData {
  title: string,
  message: string,
  yesText?: string,
  noText?: string,
  hideNoButton?: boolean
}

@Component({
  selector: 'app-confirmation-modal',
  standalone: true,
  imports: [
    MatButtonModule,
    MatDialogTitle,
    MatDialogContent,
    MatDialogActions
  ],
  templateUrl: './confirmation-modal.component.html',
  styleUrl: './confirmation-modal.component.scss'
})
export class ConfirmationModalComponent {
  readonly dialogRef = inject(MatDialogRef<ConfirmationModalComponent>);
  readonly data: IConfirmationData = inject(MAT_DIALOG_DATA);
  
  nope() {
    this.dialogRef.close(false);
  }

  ok() {
    this.dialogRef.close(true);
  }
}
