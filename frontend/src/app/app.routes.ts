import { Routes } from '@angular/router';
import {StartComponent} from "./pages/start/start.component";
import {MonopolyComponent} from "./pages/monopoly/monopoly.component";

export const routes: Routes = [
  { path: '', redirectTo: '/startseite', pathMatch: 'full' },
  { path: 'startseite', component: StartComponent, title: 'Startseite' },
  { path: 'monopolyTest', component: MonopolyComponent, title: 'Startseite' }
]

