import {Component, HostListener} from '@angular/core';
import {MatList, MatListItem} from "@angular/material/list";
import {MatFormField} from "@angular/material/form-field";
import {MatCard} from "@angular/material/card";
import {FormsModule} from "@angular/forms";
import {MatInput, MatLabel} from "@angular/material/input";
import {MatButton} from "@angular/material/button";
import {NgForOf} from "@angular/common";
import {ClientService} from "../../service/client.service";
import {Router, RouterLink} from "@angular/router";

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
    RouterLink
  ],
  templateUrl: './lobby.component.html',
  styleUrl: './lobby.component.scss'
})
export class LobbyComponent {
  players: { name: string }[] = [];

  constructor(private clientSerivce: ClientService, private router: Router) {
  }

  startGame() {
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
