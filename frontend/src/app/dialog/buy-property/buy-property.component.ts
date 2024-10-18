import {Component, Inject} from '@angular/core';
import {MatDivider} from "@angular/material/divider";
import {MatButton} from "@angular/material/button";
import {
  MAT_DIALOG_DATA,
  MatDialogActions,
  MatDialogClose,
  MatDialogContent, MatDialogRef,
  MatDialogTitle
} from "@angular/material/dialog";
import {NgIf} from "@angular/common";

@Component({
  selector: 'app-buy-property',
  standalone: true,
  imports: [
    MatDivider,
    MatButton,
    NgIf,
    MatDialogTitle,
    MatDialogContent,
    MatDialogActions,
    MatDialogClose
  ],
  templateUrl: './buy-property.component.html',
  styleUrl: './buy-property.component.scss'
})
export class BuyPropertyComponent {
  constructor(
    private dialogRef: MatDialogRef<BuyPropertyComponent>,
    @Inject(MAT_DIALOG_DATA) public data: {name: string, price: number}
  ) {
    this.dialogRef.disableClose = true;
  }
}
