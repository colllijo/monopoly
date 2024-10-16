import { Room, Player } from './models';

export interface CreateRoom {
  user: string;
  name: string;
}

export interface CreateRoomResponse {
  player: Player;
  room: Room;
}

export interface JoinRoom {
  user: string;
}

export interface JoinRoomResponse {
  player: Player;
  room: Room;
}

export interface GetRoomByPlayerId {
  name: string;
  queue: number;
  data: {
    playerId: string;
  }
}

export type GetRoomByPlayerIdResponse = Room;
