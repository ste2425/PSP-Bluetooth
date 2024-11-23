import { Component, Input } from '@angular/core';
import { ButtonMapping, IControllerMapping } from '../services/btconnection-factory.service';
import { CommonModule } from '@angular/common';
import { MatDividerModule } from '@angular/material/divider';
import {MatGridListModule} from '@angular/material/grid-list';
import { ControllerButtonMappingComponent } from '../controller-button-mapping/controller-button-mapping.component';
import { PspViewerComponent } from "../psp-viewer/psp-viewer.component";
import { pspButtons } from '../services/ESPValueDefinitions';

@Component({
  selector: 'app-controller-maping',
  standalone: true,
  imports: [CommonModule, MatDividerModule, MatGridListModule, ControllerButtonMappingComponent, PspViewerComponent],
  templateUrl: './controller-maping.component.html',
  styleUrl: './controller-maping.component.scss'
})
export class ControllerMapingComponent {
  @Input({ required: true }) controllerMapping?: IControllerMapping;

  active = pspButtons.analogDown;

  activeMapping?: ButtonMapping;

  onChange(value: number) {
    if (this.activeMapping) {
      this.activeMapping[0] = value;
    }
  }
}
