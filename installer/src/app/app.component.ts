import { Component } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { HeaderComponent } from './header/header.component';
import { MatDialog, MatDialogModule } from '@angular/material/dialog';
import { WebSerialUnsupportedComponent } from './web-serial-unsupported/web-serial-unsupported.component';
import { SerialConnectionFactoryService } from './services/serial-connection-factory.service';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [RouterOutlet, HeaderComponent, MatDialogModule],
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  constructor(private dialog: MatDialog, public serialService: SerialConnectionFactoryService) {}

  ngOnInit() {
    if (!this.serialService.isSupported()) {
      this.dialog.open(WebSerialUnsupportedComponent, {
        panelClass: 'dialog-width'
      });
    }
  }
}