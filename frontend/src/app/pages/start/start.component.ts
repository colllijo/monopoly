import { Component } from '@angular/core';
import { MatTableModule } from '@angular/material/table';
import { MatPaginatorModule } from '@angular/material/paginator';
import { MatSortModule } from '@angular/material/sort';
import {MatIcon} from "@angular/material/icon";
import {MatButton, MatFabButton} from "@angular/material/button";
import {FormsModule} from "@angular/forms";
import {NgForOf} from "@angular/common";
import {MatCard} from "@angular/material/card";
import {MatList, MatListItem} from "@angular/material/list";
import {MatFormField, MatInput, MatLabel} from "@angular/material/input";
import {CreateGameComponent} from "../../dialog/create-game/create-game.component";
import {MatDialog} from "@angular/material/dialog";

@Component({
  selector: 'app-start',
  templateUrl: './start.component.html',
  styleUrl: './start.component.scss',
  standalone: true,
  imports: [MatTableModule, MatPaginatorModule, MatSortModule, MatIcon, MatFabButton, FormsModule, NgForOf, MatCard, MatList, MatListItem, MatButton, MatInput, MatLabel, MatFormField]
})
export class StartComponent {
  playerName: string = '';
  games: { name: string, creator: string }[] = [
    { name: 'Game 1', creator: 'Alice' },
    { name: 'Game 2', creator: 'Bob' }
  ];

  constructor(public dialog: MatDialog) {}

  createNewGame() {
    if (this.playerName) {
      const dialogRef = this.dialog.open(CreateGameComponent, {
        width: '250px',
        data: { playerName: this.playerName }
      });

      dialogRef.afterClosed().subscribe(result => {
        if (result) {
          const newGame = { name: result, creator: this.playerName };
          this.games.push(newGame);
        }
      });
    } else {
      alert('Please enter your name to create a game.');
    }
  }

}
