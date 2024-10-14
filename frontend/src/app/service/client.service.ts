import {inject, Injectable} from '@angular/core';
import {HttpClient} from "@angular/common/http";
import {CreateRoom} from "../model/createRoom";
import {Observable} from "rxjs";
import {Rooms} from "../model/rooms";

@Injectable({
  providedIn: 'root'
})
export class ClientService {
  private readonly httpClient = inject(HttpClient);


  public getRooms(): Observable<Array<Rooms>> {
    return this.httpClient.get<Array<Rooms>>('http://localhost:8080/games');
  }

  public createRoom(createRoom: CreateRoom): Observable<Rooms> {
    return this.httpClient.post<Rooms>('http://localhost:8080/games', JSON.stringify(createRoom));
  }
}
