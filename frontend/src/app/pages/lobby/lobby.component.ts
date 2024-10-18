import { Component, HostListener, OnInit } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { MatButton } from '@angular/material/button';
import { MatCard } from '@angular/material/card';
import { MatFormField } from '@angular/material/form-field';
import { MatInput, MatLabel } from '@angular/material/input';
import { MatList, MatListItem } from '@angular/material/list';
import { MatProgressSpinner } from '@angular/material/progress-spinner';
import { Router, RouterLink } from '@angular/router';
import {
    GetRoomByPlayerId,
    GetRoomByPlayerIdResponse,
    JoinRoomResponse,
    StartGame,
} from '../../model/commands';
import { WebsocketService } from '../../service/websocket.service';

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
    RouterLink,
    MatProgressSpinner,
  ],
  templateUrl: './lobby.component.html',
  styleUrl: './lobby.component.scss',
})
export class LobbyComponent implements OnInit {
  players: { name: string }[] = [];
  playerCount: number = 1;

  constructor(
    private router: Router,
    private websocketService: WebsocketService,
  ) {}

  ngOnInit(): void {
    const getGame: GetRoomByPlayerId = {
      name: 'GetRoomByPlayerId',
      queue: 1,
      data: {
        playerId: sessionStorage.getItem('playerId') || '',
      },
    };

    this.websocketService.sendCommand(getGame).subscribe((response) => {
      this.playerCount = (response as GetRoomByPlayerIdResponse).players;
    });
    this.websocketService.getMessages()?.subscribe((message) => {
      if (message.pushType === 'JoinRoom') {
        this.playerCount = (message as JoinRoomResponse).room.players;
      }

      if (message.pushType === 'StartGame') {
        this.router.navigate(['monopoly']);
      }
    });
  }

  startGame() {
    const startGame: StartGame = {
      name: 'StartGame',
      queue: 1,
      data: {
        roomId: sessionStorage.getItem('roomId') || '',
      },
    };

    console.log('Game started with players:', this.players);
    this.websocketService.sendMessage(startGame);
  }

  leaveGame() {
    this.router.navigate(['startseite']);
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
