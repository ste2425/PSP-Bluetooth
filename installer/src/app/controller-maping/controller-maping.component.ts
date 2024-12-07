import { Component, Input } from '@angular/core';
import { ButtonMapping, IControllerMapping } from '../services/btconnection-factory.service';
import { CommonModule } from '@angular/common';
import { MatDividerModule } from '@angular/material/divider';
import {MatGridListModule} from '@angular/material/grid-list';
import { ControllerButtonMappingComponent } from '../controller-button-mapping/controller-button-mapping.component';
import { PspViewerComponent } from "../psp-viewer/psp-viewer.component";
import { DS4ViewerComponent, IDS4ViewerChange } from '../ds4-viewer/ds4-viewer.component';
import { analogControllerBits, controllerTypes, pspButtons } from '../services/ESPValueDefinitions';
import { MatIconModule } from '@angular/material/icon';
import { MatButtonModule } from '@angular/material/button';

@Component({
  selector: 'app-controller-maping',
  standalone: true,
  imports: [MatIconModule, MatButtonModule, CommonModule, MatDividerModule, MatGridListModule, ControllerButtonMappingComponent, PspViewerComponent, DS4ViewerComponent],
  templateUrl: './controller-maping.component.html',
  styleUrl: './controller-maping.component.scss'
})
export class ControllerMapingComponent {
  @Input({ required: true }) controllerMapping?: IControllerMapping;

  activeMapping?: ButtonMapping;

  get isAnalogStickMapping() {
    if (!this.activeMapping)
      return false;

    const pspButton = this.activeMapping[0],
      controllerPin = this.activeMapping[1],
      type = this.activeMapping[2];

    return pspButton === pspButtons.analog || (type === controllerTypes.analog && (controllerPin === analogControllerBits.leftAnalog || controllerPin === analogControllerBits.rightAnalog));
  }

  disableAllButAnalog() {
    if (!this.activeMapping)
      return false;

    const controllerBit = this.activeMapping[1],
      type = this.activeMapping[2];

    return type === controllerTypes.analog && (controllerBit === analogControllerBits.leftAnalog || controllerBit === analogControllerBits.rightAnalog);
  }

  disableAllControllerButAnalog() {
    if (!this.activeMapping)
      return false;

    return this.activeMapping[0] === pspButtons.analog;
  }

  addNewMapping() {
    if (!this.controllerMapping?.m || this.controllerMapping.m.length >= 16)
      return;

    const newMapping: ButtonMapping = [0, 0, 0];

    this.controllerMapping.m.push(newMapping);
    this.activeMapping = newMapping;
  }

  removeMapping(mapping: ButtonMapping) {
    const index = this.controllerMapping?.m?.indexOf(mapping);

    if (index !== undefined && index !== null && index > -1) {
      this.controllerMapping?.m?.splice(index, 1);
    }

    if (this.activeMapping === mapping)
      this.activeMapping = undefined;
  }

  onChange(value: number) {
    if (this.activeMapping) {
      if (value === 105 || this.activeMapping[2] === 3) { // psp analogstick
        //hard code toleft analog stick on psp.
        this.activeMapping[0] = 105;
        this.activeMapping[1] = 1;
        this.activeMapping[2] = 3;
      } else {
        this.activeMapping[0] = value;
      }
    }
  }

  onControllerChange(value: IDS4ViewerChange) {
    if (this.activeMapping) {
      if (this.activeMapping[0] === 105) {
        this.activeMapping[1] = value.type === 3 && (value.controllerBit === 1 || value.controllerBit === 2) ? value.controllerBit : 1;
        this.activeMapping[2] = 3;
      } else {
        this.activeMapping[1] = value.controllerBit;
        this.activeMapping[2] = value.type;
  
        if (value.pspAnalog) {
          this.activeMapping[0] = 105;
        }
      }
    }
  }
}
