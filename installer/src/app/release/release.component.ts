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
  MatDialogModule
} from '@angular/material/dialog';
import { ProgrammerComponent } from '../programmer/programmer.component';
import { SerialConnectionFactoryService } from '../services/serial-connection-factory.service';
import { MatMenuModule } from '@angular/material/menu';
import { OTAProgrammerComponent } from '../otaprogrammer/otaprogrammer.component';
import { BTConnectionFactoryService } from '../services/btconnection-factory.service';

export interface ReleaseArtifact {
  name: string,
  downloadUrl: string,
  id: number,
  created: string
}

@Component({
  selector: 'app-release',
  standalone: true,
  imports: [MatMenuModule, MatIconModule, MatDividerModule, MatListModule, MarkdownComponent, CommonModule, MatButtonModule, MatDialogModule],
  templateUrl: './release.component.html',
  styleUrl: './release.component.scss'
})
export class ReleaseComponent
{
  constructor(private dialog: MatDialog, private serialService: SerialConnectionFactoryService, private btService: BTConnectionFactoryService) {}

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

  get programDisabled() {
    return !this.serialService.isSupported();
  }

  get otaDisabled() {
    return !this.artifacts.some(a => a.name.endsWith('-ota.bin')) || !this.btService.isSupported();
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

  programOTA() {
    if (!this.release)
      return;

    const asset = this.release.assets.find(x => x.name === 'pspBluetooth-ota.bin');

    if (!asset)
      return;

    this.dialog.open(OTAProgrammerComponent, {
      data: {
        binaryUrl: asset.browser_download_url,
        assetId: asset.id,
        tag: this.release.tag_name
      },
      panelClass: 'dialog-width'
    });
  }
 }
