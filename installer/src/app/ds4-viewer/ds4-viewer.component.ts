import { CommonModule } from '@angular/common';
import { Component, EventEmitter, Input, Output, SimpleChanges } from '@angular/core';
import { analogControllerBits, availablePspButtons, buttonControllerBits, controllerTypes, dpadControllerBits, miscControllerBits, pspButtons } from '../services/ESPValueDefinitions';
import { ButtonMapping } from '../services/btconnection-factory.service';

enum ControllerButton {
  dpadUp,
  dpadDown,
  dpadLeft,
  dpadRight,
  share,
  options,
  ps,
  cross,
  square,
  circle,
  triangle,
  l1,
  l2,
  r1,
  r2,
  leftAnalog,
  rightAnalog
}

export interface IDS4ViewerChange {
  type: number,
  controllerBit: number,
  pspAnalog?: boolean
}

@Component({
  selector: 'app-ds4-viewer',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './ds4-viewer.component.html',
  styleUrl: './ds4-viewer.component.scss'
})
export class DS4ViewerComponent {
  ControllerButton = ControllerButton;

  @Input({ required: true }) currentlyActive?: ButtonMapping = undefined;
  @Input() disableAll = false;
  @Output() onChange = new EventEmitter<IDS4ViewerChange>();
  
  toggle(pin: ControllerButton) {
    if(this.disableAll)
      return;
    
    const event: IDS4ViewerChange = {
      type: 0,
      controllerBit: 0
    };

    switch(pin) {
      case ControllerButton.dpadUp:
        event.type = controllerTypes.dpad;
        event.controllerBit = dpadControllerBits.dpadUp;
        break;
      case ControllerButton.dpadDown:
        event.type = controllerTypes.dpad;
        event.controllerBit = dpadControllerBits.dpadDown;
        break;
      case ControllerButton.dpadLeft:
        event.type = controllerTypes.dpad;
        event.controllerBit = dpadControllerBits.dpadLeft;
        break;
      case ControllerButton.dpadRight:
        event.type = controllerTypes.dpad;
        event.controllerBit = dpadControllerBits.dpadRight;
        break;
      case ControllerButton.options:
        event.type = controllerTypes.misc;
        event.controllerBit = miscControllerBits.miscSelect;
        break;
      case ControllerButton.share:
        event.type = controllerTypes.misc;
        event.controllerBit = miscControllerBits.miscStart;
        break;
      case ControllerButton.ps:
        event.type = controllerTypes.misc;
        event.controllerBit = miscControllerBits.miscHome;
        break;
      case ControllerButton.cross:
        event.type = controllerTypes.button;
        event.controllerBit = buttonControllerBits.crossButton;
        break;
      case ControllerButton.circle:
        event.type = controllerTypes.button;
        event.controllerBit = buttonControllerBits.circleButton;
        break;
      case ControllerButton.square:
        event.type = controllerTypes.button;
        event.controllerBit = buttonControllerBits.squareButton;
        break;
      case ControllerButton.triangle:
        event.type = controllerTypes.button;
        event.controllerBit = buttonControllerBits.triangleButton;
        break;
      case ControllerButton.leftAnalog:
        event.type = controllerTypes.analog;
        event.controllerBit = analogControllerBits.leftAnalog;
        event.pspAnalog = true;
        break;
      case ControllerButton.rightAnalog:
        event.type = controllerTypes.analog;
        event.controllerBit = analogControllerBits.rightAnalog;
        event.pspAnalog = true;
        break;
      case ControllerButton.l1:
        event.type = controllerTypes.button;
        event.controllerBit = buttonControllerBits.l1;
        break;
      case ControllerButton.l2:
        event.type = controllerTypes.button;
        event.controllerBit = buttonControllerBits.l2;
        break;
      case ControllerButton.r1:
        event.type = controllerTypes.button;
        event.controllerBit = buttonControllerBits.r1;
        break;
      case ControllerButton.r2:
        event.type = controllerTypes.button;
        event.controllerBit = buttonControllerBits.r2;
        break;
      default:
        throw new Error('Bit not found');
    }

    this.onChange.next(event);
  }

  isActive(pin: ControllerButton) {
    const type = this.currentlyActive?.[2],
      controllerBit = this.currentlyActive?.[1];

    switch(pin) {
      case ControllerButton.dpadUp:
        return type === controllerTypes.dpad && controllerBit === dpadControllerBits.dpadUp;
      case ControllerButton.dpadDown:
        return type === controllerTypes.dpad && controllerBit === dpadControllerBits.dpadDown;
      case ControllerButton.dpadLeft:
        return type === controllerTypes.dpad && controllerBit === dpadControllerBits.dpadLeft;
      case ControllerButton.dpadRight:
        return type === controllerTypes.dpad && controllerBit === dpadControllerBits.dpadRight;
      case ControllerButton.options:
        return type === controllerTypes.misc && controllerBit === miscControllerBits.miscSelect;
      case ControllerButton.share:
        return type === controllerTypes.misc && controllerBit === miscControllerBits.miscStart;
      case ControllerButton.ps:
        return type === controllerTypes.misc && controllerBit === miscControllerBits.miscHome;
      case ControllerButton.cross:
        return type === controllerTypes.button && controllerBit === buttonControllerBits.crossButton;
      case ControllerButton.circle:
        return type === controllerTypes.button && controllerBit === buttonControllerBits.circleButton;
      case ControllerButton.square:
        return type === controllerTypes.button && controllerBit === buttonControllerBits.squareButton;
      case ControllerButton.triangle:
        return type === controllerTypes.button && controllerBit === buttonControllerBits.triangleButton;
      case ControllerButton.leftAnalog:
        return type === controllerTypes.analog && controllerBit === analogControllerBits.leftAnalog;
      case ControllerButton.rightAnalog:
        return type === controllerTypes.analog && controllerBit === analogControllerBits.rightAnalog;
      case ControllerButton.l1:
        return type === controllerTypes.button && controllerBit === buttonControllerBits.l1;
      case ControllerButton.l2:
        return type === controllerTypes.button && controllerBit === buttonControllerBits.l2;
      case ControllerButton.r1:
        return type === controllerTypes.button && controllerBit === buttonControllerBits.r1;
      case ControllerButton.r2:
        return type === controllerTypes.button && controllerBit === buttonControllerBits.r2;
      default:
        return false;
    }
  }

  //[ngClass]="{ active: isActive(ControllerButton.voldown) }"
}
