import {Component, OnInit} from '@angular/core';
import { MatTableModule } from '@angular/material/table';
import { MatPaginatorModule } from '@angular/material/paginator';
import { MatSortModule } from '@angular/material/sort';
import {MatIcon} from "@angular/material/icon";
import {MatButton, MatFabButton} from "@angular/material/button";
import {FormsModule} from "@angular/forms";
import {NgForOf} from "@angular/common";
import {MatCard} from "@angular/material/card";
import {MatList, MatListItem} from "@angular/material/list";
import {MatInput, MatLabel} from "@angular/material/input";
import {CreateGameComponent} from "../../dialog/create-game/create-game.component";
import {MatDialog} from "@angular/material/dialog";
import {ClientService} from "../../service/client.service";
import {Rooms} from "../../model/rooms";
import {MatFormField} from "@angular/material/form-field";
import {Router} from "@angular/router";

@Component({
  selector: 'app-start',
  templateUrl: './start.component.html',
  styleUrl: './start.component.scss',
  standalone: true,
  imports: [MatTableModule, MatPaginatorModule, MatSortModule, MatIcon, MatFabButton, FormsModule, NgForOf, MatCard, MatList, MatListItem, MatButton, MatInput, MatLabel, MatFormField]
})
export class StartComponent implements OnInit{
  playerName: string = '';
  rooms : Array<Rooms> = new Array<Rooms>()

  constructor(public dialog: MatDialog, private clientService: ClientService, private router: Router
  ) {
    clientService.getRooms()
  }

  joinRoom(user: string, gameId: string, name: string) {
    if (this.playerName) {
      this.clientService.joinRoom(user, gameId).subscribe()
      this.router.navigate(['lobby/' + name])
    } else {
      alert('Gib deinen Namen ein bevor du einem Spiel beitritts');
    }
  }

  createNewRoom() {
    if (this.playerName) {
      const dialogRef = this.dialog.open(CreateGameComponent, {
        width: '250px',
        data: { playerName: this.playerName }
      });
      dialogRef.afterClosed().subscribe(result => {
        if (result) {
          let createRoom = { user: this.playerName, name: result };
          this.clientService.createRoom(createRoom).subscribe()
        }
      });
    } else {
      alert('Gib deinen Namen ein bevor du ein neues Spiel erstellst.');
    }
  }

  ngOnInit(): void {
    this.clientService.getRooms().subscribe(obj => {
      this.rooms = obj
    })  }

}
