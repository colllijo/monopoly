import { Field, Game, Room, Player } from './models';

export interface CreateRoom {
  user: string;
  name: string;
};

export interface CreateRoomResponse {
  player: Player;
  room: Room;
};

export interface JoinRoom {
  user: string;
};

export interface JoinRoomResponse {
  player: Player;
  room: Room;
};

export interface GetRoomByPlayerId {
  name: string;
  queue: number;
  data: {
    playerId: string;
  }
};

export type GetRoomByPlayerIdResponse = Room;

export interface StartGame {
  name: string;
  queue: number;
  data: {
    roomId: string;
  }
};

export type StartGameResponse = Game;

export interface GetPlayersByRoomId {
  name: string;
  queue: number;
  data: {
    roomId: string;
  }
};

export interface GetPlayersByRoomIdResponse {
  players: Player[];
};

export interface GetGameByRoomId {
  name: string;
  queue: number;
  data: {
    roomId: string;
  }
};

export type GetGameByRoomIdResponse = Game;

export interface PlayerStartTurn {
  name: string;
  queue: number;
  data: {
    playerId: string;
    roomId: string;
  };
};

export type PlayerStartTurnResponse = Field;

export interface PlayerEndTurn {
  name: string;
  queue: number;
  data: {
    playerId: string;
    roomId: string;
  };
}

export type PlayerEndTurnResponse = Game;

export interface PlayerBuyField {
  name: string;
  queue: number;
  data: {
    playerId: string;
    roomId: string;
  };
}
