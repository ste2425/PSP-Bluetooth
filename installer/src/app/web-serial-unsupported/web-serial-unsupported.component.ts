import { Component } from '@angular/core';
import { MatDialogModule } from '@angular/material/dialog';
import {MatButtonModule} from '@angular/material/button';
@Component({
  selector: 'app-web-serial-unsupported',
  standalone: true,
  imports: [MatDialogModule, MatButtonModule],
  templateUrl: './web-serial-unsupported.component.html',
  styleUrl: './web-serial-unsupported.component.scss'
})
export class WebSerialUnsupportedComponent {}
