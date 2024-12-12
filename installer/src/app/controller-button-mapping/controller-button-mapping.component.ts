import { CommonModule } from '@angular/common';
import { Attribute, Component, EventEmitter, HostBinding, HostListener, inject, Input, Output, output } from '@angular/core';
import { analogControllerBits, buttonControllerBits, controllerTypes, dpadControllerBits, miscControllerBits, pspButtons } from '../services/ESPValueDefinitions';
import {FormControl, FormsModule, ReactiveFormsModule} from '@angular/forms';
import {MatSelectModule} from '@angular/material/select';
import {MatFormFieldModule} from '@angular/material/form-field';
import { TourMatMenuModule, TourService } from 'ngx-ui-tour-md-menu';
import { TimmyDirective } from '../timmy.directive';

const dpadDisplayValues = {
  [dpadControllerBits.dpadUp]: 'DPAD up',
  [dpadControllerBits.dpadDown]: 'DPAD down',
  [dpadControllerBits.dpadLeft]: 'DPAD left',
  [dpadControllerBits.dpadRight]: 'DPAD right'
};

const analogDisplayValues = {
  [analogControllerBits.leftAnalog]: 'Left Analog',
  [analogControllerBits.leftAnalogLeft]: 'Left Analog left',
  [analogControllerBits.leftAnalogRight]: 'Left Analog right',
  [analogControllerBits.leftAnalogUp]: 'Left Analog up',
  [analogControllerBits.leftAnalogDown]: 'Left Analog down',
  [analogControllerBits.rightAnalog]: 'Right Analog',
  [analogControllerBits.rightAnalogLeft]: 'Right Analog left',
  [analogControllerBits.rightAnalogRight]: 'Right Analog right',
  [analogControllerBits.rightAnalogUp]: 'Right Analog up',
  [analogControllerBits.rightAnalogDown]: 'Right Analog down'
}

const buttonDisplayValues = {
  [buttonControllerBits.crossButton]: 'Cross button',
  [buttonControllerBits.circleButton]: 'Circle button',
  [buttonControllerBits.triangleButton]: 'Triangle button',
  [buttonControllerBits.squareButton]: 'Square button',
  [buttonControllerBits.l1]: 'L1 button',
  [buttonControllerBits.l2]: 'L2 button',
  [buttonControllerBits.r1]: 'R1 button',
  [buttonControllerBits.r2]: 'R2 button',
  [buttonControllerBits.thumbR]: 'Right Thumbstick button',
  [buttonControllerBits.thumbL]: 'Left Thumbstick button'
};

const miscDisplayValues = {
  [miscControllerBits.miscHome]: 'PS button',
  [miscControllerBits.miscCapture]: 'Capture button',
  [miscControllerBits.miscSelect]: 'Capture* button',
  [miscControllerBits.miscStart]: 'Options button'
}

const pspDisplayValues = {
  [pspButtons.up]: 'DPAD up',
  [pspButtons.down]: 'DPAD down',
  [pspButtons.left]: 'DPAD left',
  [pspButtons.right]: 'DPAD right',
  [pspButtons.cross]: 'Cross button',
  [pspButtons.circle]: 'Circle button',
  [pspButtons.triangle]: 'Triangle button',
  [pspButtons.square]: 'Square button',
  [pspButtons.l1]: 'L1 button',
  [pspButtons.r1]: 'R1 button',
  [pspButtons.analogLeft]: 'Thumb Stick left',
  [pspButtons.analogRight]: 'Thumb Stick right',
  [pspButtons.analogUp]: 'Thumb Stick up',
  [pspButtons.analogDown]: 'Thumb Stick down',
  [pspButtons.power]: 'Power Switch',
  [pspButtons.sound]: 'Sound button',
  [pspButtons.display]: 'Display button',
  [pspButtons.volup]: 'Volume Up button',
  [pspButtons.voldown]: 'Volume Down button',
  [pspButtons.home]: 'Home button',
  [pspButtons.start]: 'Start button',
  [pspButtons.select]: 'Select button'
};

interface IControllerOption {
  label: string,
  controllerType: number,
  controllerBit: number
}

@Component({
  selector: 'app-controller-button-mapping',
  standalone: true,
  imports: [TourMatMenuModule, CommonModule, MatFormFieldModule, MatSelectModule, FormsModule, ReactiveFormsModule],
  templateUrl: './controller-button-mapping.component.html',
  styleUrl: './controller-button-mapping.component.scss'
})
export class ControllerButtonMappingComponent {
  tourService = inject(TourService);
  
  ngOnDestroy() {
    //this.tourService.unregister(this.tour);
    console.log('DESTROY');
  }

  @Input() tour: string = '';

  @Input({ required: true }) mapping?: [number, number, number]; //psp, controller bit, controller type

  controllerOptions: IControllerOption[] = [
    {
      label: 'DPAD up',
      controllerType: controllerTypes.dpad,
      controllerBit: dpadControllerBits.dpadUp
    },
    {
      label: 'DPAD down',
      controllerType: controllerTypes.dpad,
      controllerBit: dpadControllerBits.dpadDown
    },
    {
      label: 'DPAD left',
      controllerType: controllerTypes.dpad,
      controllerBit: dpadControllerBits.dpadLeft
    },
    {
      label: 'DPAD right',
      controllerType: controllerTypes.dpad,
      controllerBit: dpadControllerBits.dpadRight
    }
  ];

  onSelected(value: IControllerOption) {
    console.log(value);
  }

  get controllerButtonText() {
    if (!this.mapping)
      return '';

    const type = this.mapping[2],
      controllerBit = this.mapping[1];
    
    if (type === controllerTypes.dpad)
      return dpadDisplayValues[controllerBit];

    if (type === controllerTypes.button)
      return buttonDisplayValues[controllerBit];

    if (type === controllerTypes.misc)
      return miscDisplayValues[controllerBit];

    if (type === controllerTypes.analog)
      return analogDisplayValues[controllerBit];

    return '';
  }

  get pspButtonText() {
    if (!this.mapping)
      return '';

    const pspButton = this.mapping[0],
      controllerBit = this.mapping[1],
      controllerType = this.mapping[2];

    const isAnalog = controllerBit === analogControllerBits.leftAnalog || analogControllerBits.rightAnalog;

    // Analog sticks (full 360 not a specific direction) can only be mapped to the PSP's analog stick so hard code it
    if (controllerType === controllerTypes.analog && isAnalog)
      return 'Analog stick';

    return pspDisplayValues[pspButton];
  }
}
