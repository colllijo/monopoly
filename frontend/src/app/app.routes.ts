import { Routes } from '@angular/router';
import {StartComponent} from "./pages/start/start.component";
import {MonopolyComponent} from "./pages/monopoly/monopoly.component";
import {LobbyComponent} from "./pages/lobby/lobby.component";

export const routes: Routes = [
  { path: '', redirectTo: '/startseite', pathMatch: 'full' },
  { path: 'startseite', component: StartComponent, title: 'Startseite' },
  { path: 'lobby/:gameName', component: LobbyComponent, title: 'Lobby:/gameName' },
  { path: 'monopoly', component: MonopolyComponent, title: 'Monopoly' }
]

