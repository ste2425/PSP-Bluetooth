import { Routes } from '@angular/router';
import { HomePageComponent } from './pages/home-page/home-page.component';

export const routes: Routes = [
{
    path: '',
    component: HomePageComponent,
    pathMatch: 'full'
},
{
    path: 'configuration',
    loadComponent: () => import('./pages/configuration-page/configuration-page.component').then(m => m.ConfigurationPageComponent),
    pathMatch: 'full'
},
{
    path: 'serial',
    loadComponent: () => import('./pages/serial-page/serial-page.component').then(m => m.SerialPageComponent),
    pathMatch: 'full'
}, {
    path: 'program',
    loadComponent: () => import('./pages/programmer-page/programmer-page.component').then(m => m.ProgrammerPageComponent),
    pathMatch: 'full'
}, {
    path: 'settings',
    loadComponent: () => import('./pages/settings-page/settings-page.component').then(m => m.SettingsPageComponent),
    pathMatch: 'full'
}];
