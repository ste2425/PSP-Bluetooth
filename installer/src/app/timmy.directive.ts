import { Directive, Input } from '@angular/core';
import { TourService } from 'ngx-ui-tour-md-menu';

@Directive({
  selector: '[appTimmy]',
  standalone: true
})
export class TimmyDirective {

  constructor(private tourService: TourService) { }

  @Input('appTimmy') lol: string = '';

  ngOnInit() {
    this.tourService.initialize([{
      anchorId: 'config.navigateback',
      content: 'Some content',
      title: 'First',
    }, {
      anchorId: 'config.colourpicker',
      content: 'Other content',
      title: 'Second',
    }]);
  }
}
