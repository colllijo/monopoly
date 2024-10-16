import {inject, Injectable} from '@angular/core';
import {HttpClient} from "@angular/common/http";
import {CreateRoom, CreateRoomResponse, JoinRoom, JoinRoomResponse} from "../model/commands";
import {Observable} from "rxjs";
import {Room} from "../model/models";

@Injectable({
  providedIn: 'root'
})
export class ClientService {
  private readonly httpClient = inject(HttpClient);

  public getRooms(): Observable<Array<Room>> {
    return this.httpClient.get<Array<Room>>('http://localhost:8080/games');
  }

  public createRoom(createRoom: CreateRoom): Observable<CreateRoomResponse> {
    return this.httpClient.post<CreateRoomResponse>('http://localhost:8080/games', JSON.stringify(createRoom));
  }

  public joinRoom(joinRoom: JoinRoom, gameId: string): Observable<JoinRoomResponse> {
    return this.httpClient.post<JoinRoomResponse>('http://localhost:8080/games/' + gameId, JSON.stringify(joinRoom));
  }

  public leaveRoom(name: String) {
    console.log('Spieler: ' + name + ' hat die Lobby verlassen')
  }
}
