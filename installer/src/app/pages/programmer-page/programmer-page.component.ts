import { Component } from '@angular/core';
import { GithubService } from '../../github.service';
import { CommonModule } from '@angular/common';
import { ReleaseComponent } from '../../release/release.component';
import {MatProgressSpinnerModule} from '@angular/material/progress-spinner';
import { SerialConnectionFactoryService } from '../../services/serial-connection-factory.service';
import {MatCheckboxModule} from '@angular/material/checkbox';
import {FormsModule} from '@angular/forms';
import { BehaviorSubject, combineLatest, map, shareReplay, withLatestFrom } from 'rxjs';
import { PanelComponent } from '../../panel/panel.component';

@Component({
  selector: 'app-programmer-page',
  standalone: true,
  imports: [
    MatProgressSpinnerModule,
    ReleaseComponent,
    CommonModule,
    MatCheckboxModule,
    FormsModule,
    PanelComponent
  ],
  templateUrl: './programmer-page.component.html',
  styleUrl: './programmer-page.component.scss'
})
export class ProgrammerPageComponent {
  constructor(
    public githubService: GithubService,
    public serialService: SerialConnectionFactoryService
  ) {}

  $incPreRelease = new BehaviorSubject(false);
  get includePrerelease(): boolean {
    return this.$incPreRelease.value;
  }
  set includePrerelease(v: boolean) {
    this.$incPreRelease.next(v);
  }  

  $releases = combineLatest([
    this.githubService.getReleases().pipe(shareReplay(1)),
    this.$incPreRelease
  ]).pipe(
    map(([releases, incPrerelease]) => {
      return releases.filter(x => {
        if (!incPrerelease)
          return !x.prerelease;
        else
          return true;
      });
    })
  );
  
}
