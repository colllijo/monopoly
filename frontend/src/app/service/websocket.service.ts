import {Injectable} from '@angular/core';
import {webSocket, WebSocketSubject} from 'rxjs/webSocket';
import {Observable} from "rxjs";


@Injectable({
  providedIn: 'root'
})
export class WebsocketService {
  private socket$: WebSocketSubject<any>;

  constructor() {
    this.socket$ = webSocket({
      url: 'wss://echo.websocket.org',
      deserializer: ({ data }) => data
    });
  }

  sendMessage(msg: string): void {
    this.socket$.next(msg);
  }

  getMessages(): Observable<string> {
    return this.socket$.asObservable();
  }

  close(): void {
    this.socket$.complete();
  }
}
