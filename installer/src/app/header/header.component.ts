import { Component, inject } from '@angular/core';
import { ThemeManager } from '../theme-manager.service';
import { MatIconModule, MatIconRegistry } from '@angular/material/icon';
import { MatMenuModule } from '@angular/material/menu';
import { CommonModule } from '@angular/common';
import {MatButtonModule} from '@angular/material/button';
import { DomSanitizer } from '@angular/platform-browser';
import { ActivatedRoute, RouterLink, RouterLinkActive } from '@angular/router';
import {MatListModule} from '@angular/material/list';

const GITHUB_ICON =
  `
    <svg viewBox="0 0 20 20" class="github-logo" aria-hidden="true">
        <path
            d="M10 0C4.477 0 0 4.477 0 10c0 4.42 2.87 8.17 6.84 9.5.5.08.66-.23.66-.5v-1.69c-2.77.6-3.36-1.34-3.36-1.34-.46-1.16-1.11-1.47-1.11-1.47-.91-.62.07-.6.07-.6 1 .07 1.53 1.03 1.53 1.03.87 1.52 2.34 1.07 2.91.83.09-.65.35-1.09.63-1.34-2.22-.25-4.55-1.11-4.55-4.92 0-1.11.38-2 1.03-2.71-.1-.25-.45-1.29.1-2.64 0 0 .84-.27 2.75 1.02.79-.22 1.65-.33 2.5-.33.85 0 1.71.11 2.5.33 1.91-1.29 2.75-1.02 2.75-1.02.55 1.35.2 2.39.1 2.64.65.71 1.03 1.6 1.03 2.71 0 3.82-2.34 4.66-4.57 4.91.36.31.69.92.69 1.85V19c0 .27.16.59.67.5C17.14 18.16 20 14.42 20 10A10 10 0 0 0 10 0z"
            fill="currentColor" fill-rule="evenodd" />
    </svg>
`;

interface IRoute {
  path: string,
  label: string,
  experimental: boolean
}

@Component({
  selector: 'app-header',
  standalone: true,
  imports: [MatListModule, MatIconModule, MatMenuModule, CommonModule, MatButtonModule, RouterLink, RouterLinkActive],
  templateUrl: './header.component.html',
  styleUrl: './header.component.scss'
})
export class HeaderComponent {
  constructor(private route: ActivatedRoute, private themeManager: ThemeManager, iconRegistry: MatIconRegistry, sanitizer: DomSanitizer) {
    // Note that we provide the icon here as a string literal here due to a limitation in
    // Stackblitz. If you want to provide the icon from a URL, you can use:
    // `iconRegistry.addSvgIcon('thumbs-up', sanitizer.bypassSecurityTrustResourceUrl('icon.svg'));`
    iconRegistry.addSvgIconLiteral('app-github', sanitizer.bypassSecurityTrustHtml(GITHUB_ICON));
  }
  
  routes: IRoute[] = [
    {
      path: '/',
      label: 'Home',
      experimental: false
    },
    {
      path: '/configuration',
      label: 'Configuration',
      experimental: true
    },
    {
      path: '/program',
      label: 'Program',
      experimental: false
    }, {
      path: '/serial',
      label: 'Serial',
      experimental: false
    }
  ]

  isDark$ = this.themeManager.isDark$;

  changeTheme(theme: string) {
    this.themeManager.changeTheme(theme);
  }

  displayRoute(route: IRoute) {
    return !route.experimental || !!this.route.snapshot.queryParamMap.has('exp');
  }
}
