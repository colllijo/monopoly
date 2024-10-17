import { Injectable, OnDestroy } from '@angular/core';
import { Observable, of, Subject } from 'rxjs';
import { v4 as uuidv4 } from 'uuid';

@Injectable({
  providedIn: 'root'
})
export class WebsocketService implements OnDestroy {
  private socket: WebSocket | undefined;
  private messageSubject: Subject<any> = new Subject<any>();
  private pendingResponses: { [key: string]: Subject<any> } = {};

  constructor() { }

  ngOnDestroy() {
    this.socket?.close();
  }

  connect(): Promise<void> {
    return new Promise((resolve, reject) => {
      this.socket = new WebSocket(`ws://localhost:8008/${sessionStorage.getItem('playerId')}`);

      this.socket.onopen = () => { resolve(); };
      this.socket.onmessage = (event) => this.handleMessage(event);
      this.socket.onclose = () => { setTimeout(() => this.connect(), 1000); };
      this.socket.onerror = (error) => { console.error('WebSocket error:', error); };
    });
  }

  sendMessage(message: any): Observable<any> {
    if (!this.socket || this.socket.readyState !== WebSocket.OPEN) { return of(); }

    console.log('Sending message:', message);

    const correlationId = uuidv4();
    const messageWithCorrelationId = {
      ...message,
      correlationId: correlationId,
    };

    const responseSubject = new Subject<any>();
    this.pendingResponses[correlationId] = responseSubject;

    this.socket?.send(JSON.stringify(messageWithCorrelationId));

    return responseSubject.asObservable();
  }

  getMessages() {
    return this.messageSubject.asObservable();
  }

  private handleMessage(event: MessageEvent<any>) {
    const message = JSON.parse(event.data);

    if (message.correlationId && this.pendingResponses[message.correlationId]) {
      this.pendingResponses[message.correlationId].next(message);
      this.pendingResponses[message.correlationId].complete();

      delete this.pendingResponses[message.correlationId]; // Cleanup
    } else {
      this.messageSubject.next(message);
    }
  }
}
