import { Routes } from '@angular/router';
import { SerialPageComponent } from './serial-page/serial-page.component';
import { ProgrammerComponent } from './programmer/programmer.component';
import { ProgrammerPageComponent } from './programmer-page/programmer-page.component';
import { HomePageComponent } from './home-page/home-page.component';
import { ConfigurationPageComponent } from './configuration-page/configuration-page.component';

export const routes: Routes = [
{
    path: '',
    component: HomePageComponent,
    pathMatch: 'full'
},
{
    path: 'configuration',
    component: ConfigurationPageComponent,
    pathMatch: 'full'
},
{
    path: 'serial',
    component: SerialPageComponent
}, {
    path: 'program',
    component: ProgrammerPageComponent,
    pathMatch: 'full'
}];
