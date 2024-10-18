import { Component } from '@angular/core';
import {MatCard} from "@angular/material/card";
import {
  MatCell, MatCellDef,
  MatColumnDef,
  MatHeaderCell, MatHeaderCellDef,
  MatHeaderRow,
  MatHeaderRowDef,
  MatRow,
  MatRowDef,
  MatTable
} from "@angular/material/table";

interface WordList {
  name: string;
  words: string[];
}

@Component({
  selector: 'app-overview',
  standalone: true,
  imports: [
    MatCard,
    MatTable,
    MatHeaderCell,
    MatColumnDef,
    MatCell,
    MatHeaderRow,
    MatRow,
    MatRowDef,
    MatHeaderRowDef,
    MatCellDef,
    MatHeaderCellDef
  ],
  templateUrl: './overview.component.html',
  styleUrl: './overview.component.scss'
})
export class OverviewComponent {
  displayedColumns: string[] = ['name', 'words'];
  dataSource: WordList[] = [
    { name: 'Max', words: ['Chur kornplatz', 'Schaffhausen'] },
    { name: 'Julia', words: ['Freiburg Bahnhofstrasse', 'Biel Nidaugasse', 'Zürich Rennweg'] },
    { name: 'Lukas', words: [] },
    { name: 'Anna', words: ['Zürich Paradeplatz', 'Lausanne Place St-', 'St. Gallen Marktplatz'] },
  ];
}
