import { Component, HostBinding, Input } from '@angular/core';

@Component({
  selector: 'app-panel',
  standalone: true,
  imports: [],
  templateUrl: './panel.component.html',
  styleUrl: './panel.component.scss'
})
export class PanelComponent {
  @Input() rounded?: string;
  @HostBinding('class.rounded') get roundedBinding() {
    return this.rounded === '';
  }
  
  @Input() row?: string;
  @HostBinding('class.row') get rowBinding() {
    return this.row === '';
  }

  @Input() borderVertical?: string;
  @HostBinding('class.border-vertical') get borderVerticalBinding() {
    return this.borderVertical === '';
  }  

  @Input() borderHorizontal?: string;
  @HostBinding('class.border-horizontal') get borderHorizontalBinding() {
    return this.borderHorizontal === '';
  }

  @Input() colour?: 'primary' | 'accent' | undefined;
  @HostBinding('class.primary') get colourBindingPrimary() {
    return this.colour === 'primary';
  }
  @HostBinding('class.accent') get colourBindingAccent() {
    return this.colour === 'accent';
  }

  @Input() minHeight?: 'xs' | 'sm' | 'md' | 'lg' | undefined;
  @HostBinding('style.minHeight.em') get minHeightBinding() {
    if (!this.minHeight)
      return undefined;

    switch (this.minHeight) {
      case 'lg':
        return 20;
      case 'md':
        return 15;
      case 'sm':
        return 10;
      case 'xs':
        return 5;
    }
  } 
}
