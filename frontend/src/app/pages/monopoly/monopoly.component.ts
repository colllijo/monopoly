import { Component, HostListener, OnInit } from '@angular/core';
import { Field } from './field';
import { MatButton } from '@angular/material/button';
import { MatTooltip } from '@angular/material/tooltip';
import { MatDialog } from '@angular/material/dialog';
import { OverviewComponent } from '../../dialog/overview/overview.component';
import { WebsocketService } from '../../service/websocket.service';
import {
  GetGameByRoomId,
  GetGameByRoomIdResponse,
  GetPlayersByRoomIdResponse,
  PlayerEndTurn,
  PlayerStartTurn,
} from '../../model/commands';
import { Player } from '../../model/models';
import { StartTurnComponent } from '../../dialog/start-turn/start-turn.component';

@Component({
  selector: 'app-monopoly',
  standalone: true,
  imports: [MatButton, MatTooltip],
  templateUrl: './monopoly.component.html',
  styleUrl: './monopoly.component.scss',
})
export class MonopolyComponent implements OnInit {
  private dialogRef: any;

  players: Player[] = [];

  constructor(
    public dialog: MatDialog,
    private websocketService: WebsocketService,
  ) {}

  ngOnInit(): void {
    const getPlayersByRoomId = {
      name: 'GetPlayersByRoomId',
      queue: 1,
      data: {
        roomId: sessionStorage.getItem('roomId') || '',
      },
    };

    this.websocketService
      .sendCommand(getPlayersByRoomId)
      .subscribe((response) => {
        this.players = (response as GetPlayersByRoomIdResponse).players;
      });

    const getGameByRoomId: GetGameByRoomId = {
      name: 'GetGameByRoomId',
      queue: 1,
      data: {
        roomId: sessionStorage.getItem('roomId') || '',
      },
    };

    this.websocketService.sendCommand(getGameByRoomId).subscribe((response) => {
      if (
        (response as GetGameByRoomIdResponse).currentPlayer ===
        sessionStorage.getItem('playerId')
      ) {
        this.startTurn();
      }
    });

    this.websocketService.getMessages()?.subscribe((message) => {
      console.log(`Game got message: ${JSON.stringify(message)}`);

      if (message.pushType === 'Players') {
        this.players = (message as GetPlayersByRoomIdResponse).players;
      }

      if (message.pushType === 'StartTurn' || message.pushType === 'NextTurn') {
        this.startTurn();
      }
    });
  }

  startTurn() {
    const dialogRef = this.dialog.open(StartTurnComponent, { width: '250px' });

    dialogRef.afterClosed().subscribe(() => {
      const playerStartTurn: PlayerStartTurn = {
        name: 'PlayerStartTurn',
        queue: 1,
        data: {
          playerId: sessionStorage.getItem('playerId') || '',
          roomId: sessionStorage.getItem('roomId') || '',
        },
      };

      console.log('Starting turn');
      this.websocketService.sendCommand(playerStartTurn).subscribe(() => {
        // TODO: Check if the player can buy the property

        console.log('Ending turn');
        const playerEndTurn: PlayerEndTurn = {
          name: 'PlayerEndTurn',
          queue: 0,
          data: {
            playerId: sessionStorage.getItem('playerId') || '',
            roomId: sessionStorage.getItem('roomId') || '',
          },
        };

        this.websocketService.sendMessage(playerEndTurn);
      });
    });
  }

  @HostListener('document:keydown', ['$event'])
  handleKeyboardEvent(event: KeyboardEvent) {
    if (event.key === 'Tab' && !this.dialogRef) {
      this.openDialog();
    }
  }

  @HostListener('document:keyup', ['$event'])
  handleKeyboardUpEvent(event: KeyboardEvent) {
    if (event.key === 'Tab' && this.dialogRef) {
      this.closeDialog();
    }
  }

  openDialog(): void {
    this.dialogRef = this.dialog.open(OverviewComponent);
  }

  closeDialog(): void {
    if (this.dialogRef) {
      this.dialogRef.close();
      this.dialogRef = null;
    }
  }

  getFieldById(id: number): Field | undefined {
    return this.fields.find((field) => field.id === id);
  }

  fields: Field[] = [
    { name: 'Start', id: 0, price: 0 },
    { name: 'Chur Kornplatz', id: 1, price: 1200 },
    { name: 'Kanzlei', id: 2, price: 0 },
    { name: 'Schaffhausen Vordergasse', id: 3, price: 1200 },
    { name: 'Steuer', id: 4, price: 0 },
    { name: 'Vereinigte Privatbahnen', id: 5, price: 2000 },
    { name: 'Aarau Rathausplatz', id: 6, price: 2000 },
    { name: 'Chance', id: 7, price: 0 },
    { name: 'Neuenburg Place Purry', id: 8, price: 2000 },
    { name: 'Thun Hauptgasse', id: 9, price: 2400 },
    { name: 'BBZBL', id: 10, price: 0 },
    { name: 'Basel Steinen-Vorstadt', id: 11, price: 2800 },
    { name: 'Elektrizitätswerk', id: 12, price: 3000 },
    { name: 'Solothurn Hauptgasse', id: 13, price: 2800 },
    { name: 'Lugano Via Nassa', id: 14, price: 3200 },
    { name: 'Vereinigte Bergbahnen AG.', id: 15, price: 4000 },
    { name: 'Biel Nidaugasse', id: 16, price: 3600 },
    { name: 'Kanzlei', id: 17, price: 0 },
    { name: 'Freiburg Bahnhofstrasse', id: 18, price: 3600 },
    { name: 'La Chaux-de-Fonds', id: 19, price: 4000 },
    { name: 'Freier Parkplatz', id: 20, price: 0 },
    { name: 'Winterthur Bahnofplatz', id: 21, price: 4400 },
    { name: 'Chance', id: 22, price: 0 },
    { name: 'St. Gallen Marktplatz', id: 23, price: 4400 },
    { name: 'Bern Bundesplatz', id: 24, price: 4800 },
    { name: 'Überlandbahnen', id: 25, price: 4000 },
    { name: 'Luzern Wggisgasse', id: 26, price: 5200 },
    { name: 'Zürich Rennweg', id: 27, price: 5200 },
    { name: 'Wasserwerke', id: 28, price: 3000 },
    { name: 'Lausanne Rue de Bourg', id: 29, price: 5600 },
    { name: 'In die Schule', id: 30, price: 0 },
    { name: 'Basel Freiestrasse', id: 31, price: 6000 },
    { name: "Genf Rue de la Croix-d'Or", id: 32, price: 6000 },
    { name: 'Kanzlei', id: 33, price: 0 },
    { name: 'Bern Spitalgasse', id: 34, price: 6400 },
    { name: 'Vereinigte Schwebebahnen AG.', id: 35, price: 4000 },
    { name: 'Chance', id: 36, price: 0 },
    { name: 'Lausanne', id: 37, price: 7000 },
    { name: 'Steuer', id: 38, price: 0 },
    { name: 'Zürich Paradeplatz', id: 39, price: 8000 },
  ];
}
