import { Routes } from '@angular/router';
import { SerialPageComponent } from './serial-page/serial-page.component';
import { ProgrammerComponent } from './programmer/programmer.component';
import { ProgrammerPageComponent } from './programmer-page/programmer-page.component';

export const routes: Routes = [{
    path: 'serial',
    component: SerialPageComponent
}, {
    path: '',
    component: ProgrammerPageComponent,
    pathMatch: 'full'
}];
