export interface Room {
  id: string;
  name: string;
  state: string;
  players: number;
};

export interface Player {
  id: string;
  username: string;
  color: string;
  position: number;
  money: number;
};

export interface Game {
	id: string;
	roomId: string;
	currentPlayer: string;
	round: number;
};

export interface Field {
  name: string;
  type: string;
  position: number;
  cost: number;
  rent: number;
  owner: string;
}
