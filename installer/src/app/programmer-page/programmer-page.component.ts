import { Component } from '@angular/core';
import { GithubService } from '../github.service';
import { CommonModule } from '@angular/common';
import { ReleaseComponent } from '../release/release.component';
import {MatProgressSpinnerModule} from '@angular/material/progress-spinner';
import { SerialConnectionFactoryService } from '../services/serial-connection-factory.service';

@Component({
  selector: 'app-programmer-page',
  standalone: true,
  imports: [
    MatProgressSpinnerModule,
    ReleaseComponent,
    CommonModule
  ],
  templateUrl: './programmer-page.component.html',
  styleUrl: './programmer-page.component.scss'
})
export class ProgrammerPageComponent {
  constructor(
    public githubService: GithubService,
    public serialService: SerialConnectionFactoryService
  ) {}
  
  $releases = this.githubService.getReleases();
}
