import { Component, inject } from '@angular/core';
import { MatButtonModule } from '@angular/material/button';
import { MAT_DIALOG_DATA, MatDialogActions, MatDialogContent, MatDialogRef, MatDialogTitle } from '@angular/material/dialog';

export interface IOKModalData {
  title: string,
  message: string,
  okText?: string
}

@Component({
  selector: 'app-ok-modal',
  standalone: true,
  imports: [
    MatButtonModule,
    MatDialogTitle,
    MatDialogContent,
    MatDialogActions
  ],
  templateUrl: './ok-modal.component.html',
  styleUrl: './ok-modal.component.scss'
})
export class OKModalComponent {
  readonly dialogRef = inject(MatDialogRef<OKModalComponent>);
  readonly data: IOKModalData = inject(MAT_DIALOG_DATA);

  ok() {
    this.dialogRef.close(true);
  }
}
