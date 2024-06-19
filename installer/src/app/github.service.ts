import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { firstValueFrom, map } from 'rxjs';
import { Octokit } from "@octokit/core"

export interface IArtifact {
  url: string,
  id: number,
  name: string,
  browser_download_url: string
}

export interface IRelease {
  url: string,
  html_url: string,
  id: number,
  name: string,
  prerelease: boolean,
  tag_name: string,
  created_at: string,
  body: string,
  assets: IArtifact[]
}

@Injectable({
  providedIn: 'root'
})
export class GithubService {

  constructor(private httpClient: HttpClient) { }

  getReleases() {
    return this.httpClient.get<IRelease[]>('https://api.github.com/repos/ste2425/PSP-Bluetooth/releases')
      .pipe(map(releases => releases.filter(x => x.tag_name.startsWith('internal-'))))
  }

  async downloadBinary(assetId: string): Promise<string> {
    const headers = {
      Accept: 'application/octet-stream'
    };
    const $req = this.httpClient.get(`https://api.github.com/repos/ste2425/PSP-Bluetooth/releases/assets/${assetId}`, { headers, responseType: 'arraybuffer' });

    const data = await firstValueFrom($req);

    return this.#bufferToString(data);
  }

  #bufferToString(buffer: ArrayBuffer) {
    const bytes = new Uint8Array(buffer)
    return bytes.reduce((string, byte) => (string + String.fromCharCode(byte)), "")
  }
}
