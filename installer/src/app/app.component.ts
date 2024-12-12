import { Component } from '@angular/core';
import { RouterModule, RouterOutlet } from '@angular/router';
import { HeaderComponent } from './header/header.component';
import { MatDialog, MatDialogModule } from '@angular/material/dialog';
import { WebSerialUnsupportedComponent } from './web-serial-unsupported/web-serial-unsupported.component';
import { SerialConnectionFactoryService } from './services/serial-connection-factory.service';
import { BTConnectionFactoryService } from './services/btconnection-factory.service';
import { TourMatMenuModule, TourService } from 'ngx-ui-tour-md-menu';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [TourMatMenuModule, RouterModule, RouterOutlet, HeaderComponent, MatDialogModule],
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  constructor(private tourService: TourService, private dialog: MatDialog, public serialService: SerialConnectionFactoryService, public btService: BTConnectionFactoryService) { }

  ngOnInit() {
    const serialSupported = this.serialService.isSupported(),
      btSupported = this.btService.isSupported();

    if (!serialSupported || !btSupported) {
      this.dialog.open(WebSerialUnsupportedComponent, {
        panelClass: 'dialog-width'
      });
    }
    this.tourService.initialize([
      {
        anchorId: 'config.page',
        content: 'Button configuration is broken into two concepts. Button Groups and Button mappings.',
        placement: {
          horizontal: true
        },
        showArrow: false
      }, {
        anchorId: 'config.buttongroups',
        content: 'Button Groups contain Button mappings. There can be upto 3 Button groups each with 16 Button Mappings.',
        enableBackdrop: true,
        title: 'Button Groups'
      },
      {
        anchorId: 'config.navigateforward',
        content: 'You can cyle through your configured Button Groups, going forwards...',
        enableBackdrop: true,
        placement: {
          horizontal: true
        }
      },
      {
        anchorId: 'config.navigateback',
        content: 'and backwards. If you reach the end you will cycle back round.',
        enableBackdrop: true
      },
      {
        anchorId: 'config.creategroup',
        content: 'New Button Groups can be created, which will default with a single Button Mapping.',
        enableBackdrop: true,
        placement: {
          horizontal: true
        }
      },
      {
        anchorId: 'config.deletegroup',
        content: 'Button Groups can also be deleted.',
        enableBackdrop: true,
        placement: {
          horizontal: true
        }
      },
      {
        anchorId: 'config.colourpicker',
        content: 'Each Button Group has a colour assigned. If your controller has an LED the colour will be used to indicate the active Button Grouping.',
        enableBackdrop: true
      },
      {
        anchorId: 'config.buttonmapping-0',
        content: 'Button Mappings indicate a mapping between a controller button/analog-stick and a PSP button/analogstick.',
        title: 'Button Mappings',
        enableBackdrop: true
      },
      {
        anchorId: 'config.buttondelete-0',
        content: 'Button Mappings can be deleted....',
        enableBackdrop: true
      },
      {
        anchorId: 'config.buttoncreate',
        content: 'and created, with a default configuration.',
        enableBackdrop: true
      },
      {        
        anchorId: 'config.buttonmapping-0',
        content: 'Clicking on a mapping makes it active.',
        enableBackdrop: true
      },
      {        
        anchorId: 'config.mappingselector',
        content: 'With an active Button Mapping you can view how its configured. The buttons mapped will be highlighted in green.',
        enableBackdrop: true
      },
      {        
        anchorId: 'config.mappingselectords4',
        content: 'You can then click a button on the controller...',
        enableBackdrop: true
      },
      {        
        anchorId: 'config.mappingselectorpsp',
        content: 'and on the PSP to create/modify the mapping.',
        enableBackdrop: true
      },
      {        
        anchorId: 'config.savebtn',
        content: 'Finally you can save your changes. Button Groups without any Button Mapings will be removed.',
        enableBackdrop: true
      },
      {        
        anchorId: 'config.loadbtn',
        content: 'You can undo any unsaved changes by loading the config again.',
        enableBackdrop: true
      }
    ]);

  }
}