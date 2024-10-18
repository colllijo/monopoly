import { Component } from '@angular/core';
import { MatButton } from '@angular/material/button';
import { MatDialogActions, MatDialogClose, MatDialogRef, MatDialogTitle } from '@angular/material/dialog';

@Component({
  selector: 'app-start-turn',
  standalone: true,
  imports: [
    MatDialogTitle,
    MatDialogActions,
    MatDialogClose,
    MatButton,
  ],
  templateUrl: './start-turn.component.html',
  styleUrl: './start-turn.component.scss'
})
export class StartTurnComponent {
  constructor(
    public dialogRef: MatDialogRef<StartTurnComponent>,
  ) {
    this.dialogRef.disableClose = true;
  }

  onDiceRoll(): void {
    this.dialogRef.close();
  }
}
