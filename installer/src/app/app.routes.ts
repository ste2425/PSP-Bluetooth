import { Routes } from '@angular/router';
import { HomePageComponent } from './home-page/home-page.component';

export const routes: Routes = [
{
    path: '',
    component: HomePageComponent,
    pathMatch: 'full'
},
{
    path: 'configuration',
    loadComponent: () => import('./configuration-page/configuration-page.component').then(m => m.ConfigurationPageComponent),
    pathMatch: 'full'
},
{
    path: 'serial',
    loadComponent: () => import('./serial-page/serial-page.component').then(m => m.SerialPageComponent),
    pathMatch: 'full'
}, {
    path: 'program',
    loadComponent: () => import('./programmer-page/programmer-page.component').then(m => m.ProgrammerPageComponent),
    pathMatch: 'full'
}];
