import { Component } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RouterOutlet } from '@angular/router';
import {MatButtonModule} from '@angular/material/button';
import { ReleaseComponent } from './release/release.component';
import { HeaderComponent } from './header/header.component';
import {MatDividerModule} from '@angular/material/divider';
import {MatCardModule} from '@angular/material/card';
import {MatProgressBarModule} from '@angular/material/progress-bar';
import { GithubService, IRelease } from './github.service';
import { map, startWith, take } from 'rxjs';
import {MatProgressSpinnerModule} from '@angular/material/progress-spinner';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [MatProgressSpinnerModule, MatDividerModule, MatProgressBarModule, MatCardModule, HeaderComponent, ReleaseComponent, CommonModule, RouterOutlet, MatButtonModule],
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  constructor(public githubService: GithubService) {}
  
  title = 'Timmy';
  $releases = this.githubService.getReleases();
}
