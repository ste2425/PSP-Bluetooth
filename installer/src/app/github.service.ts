import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { delay, map, Observable } from 'rxjs';

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

export enum ReleaseType {
  Serial,
  OTA
}

export interface IReleaseBinary {
  data: string,
  size: number,
  buffer: ArrayBuffer
}

@Injectable({
  providedIn: 'root'
})
export class GithubService {

  constructor(private httpClient: HttpClient) { }

  getReleases() {
    return this.httpClient.get<IRelease[]>('https://api.github.com/repos/ste2425/PSP-Bluetooth/releases')
      .pipe(map(releases => releases.filter(x => x.tag_name.startsWith('release-'))))
      .pipe(delay(1500));
  }

  getReleaseBinary(releaseTag: string, type: ReleaseType): Observable<IReleaseBinary> {
    const headers = {
      Accept: 'application/octet-stream'
    };

    const binary = type === ReleaseType.Serial ? 'pspBluetooth.bin' : 'pspBluetooth-ota.bin';

    return this.httpClient.get(`https://raw.githubusercontent.com/ste2425/PSP-Bluetooth/${releaseTag}/releaseBinaries/${binary}`, { headers, responseType: 'arraybuffer' })
      .pipe(map(data => ({
        data: this.#bufferToString(data),
        size: data.byteLength,
        buffer: data
      })));
  }

  #bufferToString(buffer: ArrayBuffer) {
    const bytes = new Uint8Array(buffer)
    return bytes.reduce((string, byte) => (string + String.fromCharCode(byte)), "")
  }
}
