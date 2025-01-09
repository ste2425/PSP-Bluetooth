import { CommonModule } from '@angular/common';
import { Component, EventEmitter, Input, Output, SimpleChanges } from '@angular/core';
import { availablePspButtons, pspButtons } from '../services/ESPValueDefinitions';

@Component({
  selector: 'app-psp-viewer',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './psp-viewer.component.html',
  styleUrl: './psp-viewer.component.scss'
})
export class PspViewerComponent {
  PSPButtons = pspButtons;

  @Input({ required: true }) currentlyActivePin?: number = undefined;
  @Input() disableAll = false;
  @Input() disableAllButAnalog = false;
  @Output() onChange = new EventEmitter<number>();

  isActive(pin: number) {
    return pin === this.currentlyActivePin;
  }

  toggleActive(pin: number) {
    if (this.disableAll || (this.disableAllButAnalog && pin !== this.PSPButtons.analog))
      return;
    
    if (availablePspButtons.includes(pin)) {
      this.onChange.next(pin);
    }
  }
}
