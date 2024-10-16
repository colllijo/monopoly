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

  connect(): void {
    this.socket = new WebSocket(`ws://localhost:8008/${sessionStorage.getItem('playerId')}`);

    this.socket.onopen = () => {
      console.log('WebSocket connection established.');
    };

    this.socket.onmessage = (event) => {
      const message = JSON.parse(event.data);

      if (message.correlationId && this.pendingResponses[message.correlationId]) {
        // Resolve the associated response
        this.pendingResponses[message.correlationId].next(message);
        this.pendingResponses[message.correlationId].complete();
        delete this.pendingResponses[message.correlationId]; // Cleanup
      } else {
        // Handle messages without correlationId (pushType)
        this.messageSubject.next(message);
      }
    };

    this.socket.onclose = () => {
      console.log('WebSocket connection closed.');
      setTimeout(() => this.connect(), 1000);
    };

    this.socket.onerror = (error) => {
      console.error('WebSocket error:', error);
    };
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

  ngOnDestroy() {
    this.socket?.close();
  }
}
