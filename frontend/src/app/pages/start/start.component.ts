import { NgForOf } from '@angular/common';
import { Component, OnInit } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { MatButton, MatFabButton } from '@angular/material/button';
import { MatCard } from '@angular/material/card';
import { MatDialog } from '@angular/material/dialog';
import { MatFormField } from '@angular/material/form-field';
import { MatIcon } from '@angular/material/icon';
import { MatInput, MatLabel } from '@angular/material/input';
import { MatList, MatListItem } from '@angular/material/list';
import { MatPaginatorModule } from '@angular/material/paginator';
import { MatSortModule } from '@angular/material/sort';
import { MatTableModule } from '@angular/material/table';
import { Router } from '@angular/router';
import { CreateGameComponent } from '../../dialog/create-game/create-game.component';
import { Room } from '../../model/models';
import { ClientService } from '../../service/client.service';
import { WebsocketService } from '../../service/websocket.service';

@Component({
  selector: 'app-start',
  templateUrl: './start.component.html',
  styleUrl: './start.component.scss',
  standalone: true,
  imports: [
    MatTableModule,
    MatPaginatorModule,
    MatSortModule,
    MatIcon,
    MatFabButton,
    FormsModule,
    NgForOf,
    MatCard,
    MatList,
    MatListItem,
    MatButton,
    MatInput,
    MatLabel,
    MatFormField,
  ],
})
export class StartComponent implements OnInit {
  playerName: string = '';
  rooms: Array<Room> = new Array<Room>();
  openRooms: Array<Room> = new Array<Room>();

  constructor(
    public dialog: MatDialog,
    private clientService: ClientService,
    private router: Router,
    private websocketService: WebsocketService,
  ) {
    clientService.getRooms();
  }

  ngOnInit(): void {
    this.clientService.getRooms().subscribe((obj) => {
      this.rooms = obj;
      this.openRooms = this.rooms.filter((room) => room.state === 'LOBBY' && room.players < 4);

      console.log(this.rooms);
      console.log(this.openRooms);
    });
  }

  joinRoom(gameId: string, name: string) {
    if (!this.playerName) {
      alert('Gib deinen Namen ein bevor du ein neues Spiel erstellst.');
    }

    const joinRoom = { user: this.playerName };

    this.clientService.joinRoom(joinRoom, gameId).subscribe((res) => {
      sessionStorage.setItem('roomId', res.room.id);
      sessionStorage.setItem('playerId', res.player.id);

      this.websocketService.connect().then(() => {
        this.router.navigate(['lobby/' + name]);
      });
    });
  }

  createNewRoom() {
    if (!this.playerName) {
      alert('Gib deinen Namen ein bevor du ein neues Spiel erstellst.');
    }

    const dialogRef = this.dialog.open(CreateGameComponent, {
      width: '250px',
      data: { playerName: this.playerName },
    });

    dialogRef.afterClosed().subscribe((result) => {
      if (result) {
        const createRoom = { user: this.playerName, name: result };
        this.clientService.createRoom(createRoom).subscribe((res) => {
          sessionStorage.setItem('roomId', res.room.id);
          sessionStorage.setItem('playerId', res.player.id);

          this.websocketService.connect().then(() => {
            this.router.navigate(['lobby/' + res.room.name]);
          });
        });
      }
    });
  }
}
