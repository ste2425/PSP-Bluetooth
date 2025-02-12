import { Component, ElementRef, EventEmitter, Input, Output, TemplateRef, ViewChild } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { MatIconModule } from '@angular/material/icon';
import {MatSliderModule} from '@angular/material/slider';

@Component({
  selector: 'app-colour-picker',
  standalone: true,
  imports: [MatIconModule, FormsModule, MatSliderModule],
  templateUrl: './colour-picker.component.html',
  styleUrl: './colour-picker.component.scss'
})
export class ColourPickerComponent {
  @Input({ required: true}) R?: number;
  @Input({ required: true}) G?: number;
  @Input({ required: true}) B?: number;
  @Input({ required: true}) A?: number;

  @Output() onChange = new EventEmitter<{
    r: number,
    g: number,
    b: number,
    a: number
  }>();

  @ViewChild('input', { read: ElementRef })
  input?: ElementRef<HTMLInputElement>;

  show() {
    this.input?.nativeElement?.click();
  }
  
  get cHex() {
    const toHex = (c: number) => {
        var hex = c.toString(16);
        return hex.length == 1 ? "0" + hex : hex;
    };
    
    return `#${toHex(this.R || 0)}${toHex(this.G || 0)}${toHex(this.B || 0)}`;
  }

  emitOnChange() {
    this.onChange.next({
      r: this.R || 0,
      g: this.G || 0,
      b: this.B || 0,
      a: this.A || 0
    });
  }

  set cHex(value: string) {    
    const r = parseInt(value.slice(1, 3), 16);
    const g = parseInt(value.slice(3, 5), 16);
    const b = parseInt(value.slice(5, 7), 16);

    this.R = r;
    this.G = g;
    this.B = b;

    this.emitOnChange();
  } 
}
