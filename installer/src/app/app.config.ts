import { ApplicationConfig, Injectable } from '@angular/core';
import { provideRouter } from '@angular/router';

import { routes } from './app.routes';
import { provideAnimationsAsync } from '@angular/platform-browser/animations/async';
import { provideHttpClient } from '@angular/common/http';
import { provideMarkdown } from 'ngx-markdown';
import { TourService } from 'ngx-ui-tour-md-menu';

@Injectable({
  providedIn: 'root'
})
class ExtendedTourService extends TourService {
  // Note anchor should be of type `TourAnchorDirective` but that is not piblicly exported
  // so cannot be set.
  // Errors if dupelicate your items are reigstered. This override removes an item before adding it as a workaround
  override register(anchorId: string, anchor: any): void {
    super.unregister(anchorId);
    return super.register(anchorId, anchor);
  }
}

export const appConfig: ApplicationConfig = {
  providers: [
    {
      provide: TourService,
      useClass: ExtendedTourService
    },
    provideMarkdown(), 
    provideHttpClient(), 
    provideRouter(routes), 
    provideAnimationsAsync()
  ]
};
