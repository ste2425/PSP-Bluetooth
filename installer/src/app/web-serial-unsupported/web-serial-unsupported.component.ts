import { Component } from '@angular/core';
import { MatDialogModule } from '@angular/material/dialog';
import {MatButtonModule} from '@angular/material/button';
import { BTConnectionFactoryService } from '../services/btconnection-factory.service';
import { SerialConnectionFactoryService } from '../services/serial-connection-factory.service';
@Component({
  selector: 'app-web-serial-unsupported',
  standalone: true,
  imports: [MatDialogModule, MatButtonModule],
  templateUrl: './web-serial-unsupported.component.html',
  styleUrl: './web-serial-unsupported.component.scss'
})
export class WebSerialUnsupportedComponent {
  constructor(private btService: BTConnectionFactoryService, private serialService: SerialConnectionFactoryService) {}

  get btSupported () {
    return this.btService.isSupported();
  }

  get serialSupported() {
    return this.serialService.isSupported();
  }
}
