import { CommonModule } from '@angular/common';
import { Component, Input, input } from '@angular/core';
import { MatIconModule } from '@angular/material/icon';
import { MarkdownComponent } from 'ngx-markdown';
import {MatButtonModule} from '@angular/material/button';
import {MatDividerModule} from '@angular/material/divider';
import {MatListModule} from '@angular/material/list';
import { IArtifact, IRelease } from '../github.service';
import {
  MatDialog,
  MAT_DIALOG_DATA,
  MatDialogRef,
  MatDialogTitle,
  MatDialogContent,
  MatDialogActions,
  MatDialogClose,
} from '@angular/material/dialog';
import { ProgrammerComponent } from '../programmer/programmer.component';

export interface ReleaseArtifact {
  name: string,
  downloadUrl: string,
  id: number,
  created: string
}

@Component({
  selector: 'app-release',
  standalone: true,
  imports: [MatIconModule, MatDividerModule, MatListModule, MarkdownComponent, CommonModule, MatButtonModule],
  templateUrl: './release.component.html',
  styleUrl: './release.component.scss'
})
export class ReleaseComponent
{
  constructor(private dialog: MatDialog) {}

  @Input() release: IRelease | undefined;

  get name() {
    return this.release?.name;
  }

  get description() {
    return this.release?.body;
  }

  get createdDate() {
    return this.release?.created_at;
  }

  get releaseUrl() {
    return this.release?.html_url;
  }

  get artifacts() {
    return this.release?.assets || [];
  }

  programESP() {
    if (!this.release)
      return;

    const asset = this.release.assets.find(x => x.name === 'pspBluetooth.bin');

    if (!asset)
      return;

    this.dialog.open(ProgrammerComponent, {
      data: {
        binaryUrl: asset.browser_download_url,
        assetId: asset.id,
        tag: this.release.tag_name
      },
      panelClass: 'dialog-width'
    });
  }
 }
