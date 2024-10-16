import {Component, OnDestroy, OnInit} from '@angular/core';
import { RouterOutlet } from '@angular/router';
import {Subscription} from "rxjs";
import {WebsocketService} from "./service/websocket.service";
import {FormsModule} from "@angular/forms";
import {NgForOf} from "@angular/common";

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [RouterOutlet, FormsModule, NgForOf],
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent implements OnInit, OnDestroy{
  title = 'monopoly-frontend';

  message: string = '';
  messages: string[] = [];
  private messagesSubscription!: Subscription;

  constructor(private websocketService: WebsocketService) {}

  ngOnInit(): void {
    this.messagesSubscription = this.websocketService.getMessages().subscribe(
      (msg: any) => {
        this.messages.push('Echo: ' + msg);
      },
      (err) => console.error('WebSocket error:', err)
    );
  }

  sendMessage(): void {
    if (this.message.trim()) {
      this.websocketService.sendMessage(this.message);
      this.message = '';
    }
  }

  ngOnDestroy(): void {
    this.messagesSubscription.unsubscribe();
    this.websocketService.close();
  }
}
