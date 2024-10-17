import {Component, HostListener, OnInit} from '@angular/core';
import {MatList, MatListItem} from "@angular/material/list";
import {MatFormField} from "@angular/material/form-field";
import {MatCard} from "@angular/material/card";
import {FormsModule} from "@angular/forms";
import {MatInput, MatLabel} from "@angular/material/input";
import {MatButton} from "@angular/material/button";
import {NgForOf} from "@angular/common";
import {ClientService} from "../../service/client.service";
import {Router, RouterLink} from "@angular/router";
import { WebsocketService } from '../../service/websocket.service';
import { GetRoomByPlayerId, GetRoomByPlayerIdResponse, JoinRoomResponse } from '../../model/commands';
import {MatProgressSpinner} from "@angular/material/progress-spinner";

@Component({
  selector: 'app-lobby',
  standalone: true,
  imports: [
    MatList,
    MatListItem,
    MatFormField,
    MatCard,
    MatLabel,
    FormsModule,
    MatInput,
    MatButton,
    NgForOf,
    RouterLink,
    MatProgressSpinner
  ],
  templateUrl: './lobby.component.html',
  styleUrl: './lobby.component.scss'
})
export class LobbyComponent implements OnInit {
  players: { name: string }[] = [];
  playerCount: number = 1;

  constructor(private clientSerivce: ClientService, private router: Router, private websocketService: WebsocketService) {
  }

  ngOnInit(): void {
    const getGame: GetRoomByPlayerId = {
      name: 'GetRoomByPlayerId',
      queue: 1,
      data: {
        playerId: sessionStorage.getItem('playerId') || ''
      }
    }

    this.websocketService.sendMessage(getGame).subscribe((response) => {
      this.playerCount = (response as GetRoomByPlayerIdResponse).players
    });
    this.websocketService.getMessages()?.subscribe((message) => {
      if (message.pushType === 'JoinRoom') {
        this.playerCount = (message as JoinRoomResponse).room.players
      }
    });
  }

  startGame() {
    this.router.navigate(['monopoly'])
    console.log('Game started with players:', this.players);
  }

  leaveGame() {
    this.clientSerivce.leaveRoom('Player 1')
    this.router.navigate(['startseite'])
  }

  private inputSequence: string[] = [];
  private requiredSequence: string[] = ['g', 'o', 'a', 't'];

  @HostListener('document:keydown', ['$event'])
  handleKeyboardEvent(event: KeyboardEvent): void {
    this.inputSequence.push(event.key.toLowerCase());

    if (this.inputSequence.join('').includes(this.requiredSequence.join(''))) {
      window.open('https://messi.com/en/', '_blank');
    }

    if (this.inputSequence.length > this.requiredSequence.length) {
      this.inputSequence.shift();
    }
  }
}
