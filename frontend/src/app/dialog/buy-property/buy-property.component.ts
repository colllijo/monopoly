import {Component} from '@angular/core';
import {MatDivider} from "@angular/material/divider";
import {MatButton} from "@angular/material/button";
import {NgIf} from "@angular/common";
import {MatDialogActions, MatDialogClose, MatDialogContent, MatDialogTitle} from "@angular/material/dialog";

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
  propertyName: string = 'Bahnhof';
  propertyPrice: number = 200;

  buyProperty() {
    console.log(`Du hast das Feld ${this.propertyName} für ${this.propertyPrice} gekauft.`);
  }

  declinePurchase() {
    console.log(`Du hast den Kauf des Feldes ${this.propertyName} abgelehnt.`);
  }
}
